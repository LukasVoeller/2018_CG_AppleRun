//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "Application.h"
#include "Constants.h"
#include "GUIEvents.h"
#include "LinePlaneModel.h"
#include "TrianglePlaneModel.h"
#include "TriangleSphereModel.h"
#include "LineBoxModel.h"
#include "TriangleBoxModel.h"
#include "Model.h"
#include "ShaderLightMapper.h"
#include "DebugRender.h"

#include "CollisionHandler.h"

float toRadApp(float deg){ return deg*M_PI/180.0f; }

Application::Application(GLFWwindow* pWin) : pWindow(pWin), time(0), egocam(pWin), pModel(NULL), shadowGenerator(2048, 2048) {
	BaseModel* pModel;
	ConstantShader* pConstShader;
	pPhongShader = new PhongShader();
	
	game = Game(pWindow);
	createScene();
	
	//OutlineShader* pOutlineShader = new OutlineShader();
	
	// Create GUI
	GUIEvents gui = GUIEvents();
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Camera
	egocam.ViewMatrix().identity();
	egocam.ProjMatrix().perspective((float)M_PI*65.0f/180.0f, WINDOW_WIDTH/WINDOW_HEIGHT, 0.045f, 1000.0f);

}

void Application::start(){
    glEnable (GL_DEPTH_TEST);   // Enable depth-testing
    glDepthFunc (GL_LESS);      // Depth-testing interprets a smaller value as "closer"
	glEnable(GL_SCISSOR_TEST);	// For EgoCam
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime){
	deltaTime = calcDeltaTime();
	time += dtime;
	
	egocam.update();
	gui.update(pWindow, &egocam);
	plattformsHover();
	
	egocam.ViewMatrix() = calcCharacterViewMatrix(pCharacter);

	if(game.getCollisionHandler()->collisionWithBorder(pCharacter)) {
		pCharacter->update(deltaTime);
		return;
	}
	
	// Character steering
	game.getControl()->readInputs(this->pCharacter);
	pCharacter->steer3d(game.getControl()->getForwardBackward(), game.getControl()->getLeftRight(), game.getControl()->getJumpPower());
	pCharacter->setPosZ(0.0f);
	
	// CollisionDetections
	collisionWithBarrier();
	collisionWithBarrel();
	collisionWithCoin();
	collisionWithPallet();
	
	game.getControl()->handleJump(pCharacter);
	pCharacter->update(deltaTime);
}

// Third person cam based on inverted object matrix
Matrix Application::calcCharacterViewMatrix(Character* character) {
	Matrix characterMat = character->transform();
	Matrix matRotHorizontal, matRotVertical, matTransView;
	matTransView.translation(0, 5, 12);
	matRotHorizontal.rotationY(toRadApp(90));
	matRotVertical.rotationX(toRadApp(-10)); 		// Zum spielen auf -10 setzen
	Matrix tankViewMatrix = characterMat * matRotHorizontal * matRotVertical * matTransView;
	return tankViewMatrix.invert();
}

// Elapsed time since last call of the method
double Application::calcDeltaTime() {
    double now = glfwGetTime();
    double deltaTime = (now - this->oldTime);
    this->oldTime = now;
    if (this->oldTime == 0){
        return 1/60;	// 1/60 = 60 frames per second
    }
    return deltaTime;
}

void Application::draw() {
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw Models
	for( ModelList::iterator it = models.begin(); it != models.end(); ++it ) {
		(*it)->draw(egocam);
	}
	
	// GUI
	gui.draw(&egocam);
	
	// Util
	shadowGenerator.generate(models);
	ShaderLightMapper::instance().activate();
	ShaderLightMapper::instance().deactivate();
	
    // Check once per frame for OpenGL errors
    GLenum error = glGetError();
    assert(error==0);
}

void Application::end() {
	for( ModelList::iterator it = models.begin(); it != models.end(); ++it ) {
        delete *it;
	}
    models.clear();
}


void Application::createScene() {
	Matrix m;
	
	// Create lineGrid model with constant color shader
	//	pModel = new LinePlaneModel(10, 10, 10, 10);
	//	pConstShader = new ConstantShader();
	//	pConstShader->color(Color(1,1,1));
	//	pModel->shader(pConstShader, true);
	//	models.push_back( pModel );
	//	lineGrid = pModel;
	
	pScene = new Scene();
	pScene->shader(new PhongShader(), true);
	pScene->addSceneFile(ASSET_DIRECTORY "scene.osh");
	models.push_back(pScene);
	
	pBarriers = pScene->getObstacles();
	pCoins = pScene->getCoins();
	pDeathblocks = pScene->getDeathItems();
	pMovingItems = pScene->getMovingItems();
	
	pCharacter = new Character();
	pCharacter->shader(this->pPhongShader, true);
	pCharacter->loadModel(ASSET_DIRECTORY "android/Android.dae", 1.0f);
	m = m.translation(START_POS_X, START_POS_Y, START_POS_Z);
	pCharacter->transform(m);
	
	models.push_back(pCharacter);
	
	game.setCharacter(pCharacter);
	game.setControl(new Control(pWindow));
	
	for(NodeList::iterator it = pBarriers.begin(); it != pBarriers.end(); ++it){
		AABB bbNEU = (*it)->getScaledBoundingBox();
		bbNEU.Max.Y = 0.7*bbNEU.Max.Y;
		(*it)->setScaledBoundingBox(bbNEU);
	}
	
	 DirectionalLight* dl = new DirectionalLight();
	 dl->direction(Vector(0.2f, -1, 1));
	 dl->color(Color(0.75, 0.55, 0.55));
	 dl->castShadows(true);
	 ShaderLightMapper::instance().addLight(dl);
}

void Application::plattformsHover() {
	for(MovingItemList::iterator it = pMovingItems.begin(); it != pMovingItems.end(); ++it){
		(*it)->moving();
	}
}

void Application::collisionWithBarrier() {
	for(NodeList::iterator it = pBarriers.begin(); it != pBarriers.end(); ++it){
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(game.getCollisionHandler()->collisionDetection(pCharacter, *it, 0)) {
			coolDownTimer = 10;
			game.getCollisionHandler()->handleCollisionWithBarrier(pCharacter, *it, 0, game.getControl());
			break;
		}
		else if(pCharacter->getUnderground() == *it)
		{
			pCharacter->setUnderground(NULL);
		}
	}
}

void Application::collisionWithBarrel() {
	for(NodeList::iterator it = pDeathblocks.begin(); it != pDeathblocks.end(); ++it){
		
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(game.getCollisionHandler()->collisionDetection(pCharacter, *it, DELTA)){
			std::cout << "You died!" << std::endl;
			coolDownTimer = 10;
			gui.restartGame(); //Startbildschirm aufrufen
			game.setCollectedCoins(0);
			
			/*Setzt die Figur auf die Startposition zurück und positioniert alle Coins wieder auf die Startposition */
			game.start(pCharacter, pCoins);
			break;
		}
	}
}

void Application::collisionWithCoin() {
	for(CoinList::iterator it = pCoins.begin(); it != pCoins.end(); ++it){
		const Matrix* pCoinMat = &(*it)->getLocalTransform();
		Vector coin = (*it)->getLocalTransform().translation();
		Matrix trans;
		
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(game.getCollisionHandler()->collisionDetection(pCharacter, (*it), DELTA) && (*it)->isCollected() == false) {
			trans.translation(coin.X, coin.Y + 5.0f, coin.Z);
			(*it)->setLocalTransform(trans);
			game.foundCoin();
			coolDownTimer = 5; //Timer neu setzen
			std::cout << "found coin " << game.getCollectedCoins() << std::endl;
			game.getCollisionHandler()->handleCollisionWithCoin(*it);
		}
		if((*it)->isCollected() &&  pCoinMat->translation().Y > -20.0f) {
			game.getCollisionHandler()->handleCoinMoving(*it);
		}
	}
	
	int allCoins = ALLCOINS;
	if(game.getCollectedCoins() == allCoins) {
		gui.wonGame();
	}
}

void Application::collisionWithPallet() {
	for(MovingItemList::iterator it = pMovingItems.begin(); it != pMovingItems.end(); ++it) {
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(game.getCollisionHandler()->collisionDetection(pCharacter, *it, 0.01)) {
			coolDownTimer = 10;
			game.getCollisionHandler()->handleCollisionWithPalette(pCharacter, *it, 0.01, game.getControl());
		}
		else if(pCharacter->getPallet() == *it) {
			pCharacter->setHovering(false);
			pCharacter->setPallet(NULL);
		}
	}
}
