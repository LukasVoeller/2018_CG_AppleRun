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
	PhongShader* pPhongShader = new PhongShader();
	
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

	createScene();
	//createShadowTestScene();
	
	//OutlineShader* pOutlineShader = new OutlineShader();
	
	// Create GUI
	GUIEvents gui = GUIEvents();
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Camera
	egocam.ViewMatrix().identity();
	egocam.ProjMatrix().perspective((float)M_PI*65.0f/180.0f, WINDOW_WIDTH/WINDOW_HEIGHT, 0.045f, 1000.0f);
	
	//-------------------------- CHARACTER AND GAME LOGIC ------------------------------
	Matrix m;

	// Robot
	pCharacter = new Character();
	pPhongShader = new PhongShader();
	pCharacter->shader(pPhongShader, true);
	pCharacter->loadModel(ASSET_DIRECTORY "android/Android.dae", 1.0f);
	m = m.translation(START_POS_X, START_POS_Y, START_POS_Z);
	pCharacter->transform(m);

	game = Game(pWin);
	game.setCharacter(pCharacter);
	game.setControl(new Control(pWin));
	//game.getCollisionHandler()->setControl(game.getControl());
	
	models.push_back(pCharacter);
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
	collisionWithPallet();
	
	// Character steering
	game.getControl()->readInputs(this->pCharacter);
	pCharacter->steer3d(game.getControl()->getForwardBackward(), game.getControl()->getLeftRight(), game.getControl()->getJumpPower());
	pCharacter->setPosZ(0.0f);
	game.getControl()->handleJump(pCharacter);
	
	// CollisionDetections
	collisionWithBarrier();
	collisionWithBarrel();
	collisionWithCoin();
	
	pCharacter->update(deltaTime);
}

// Third person cam based on inverted object matrix
Matrix Application::calcCharacterViewMatrix(Character* character) {
	Matrix characterMat = character->transform();
	Matrix matRotHorizontal, matRotVertical, matTransView;
	matTransView.translation(0, 5, 12);
	matRotHorizontal.rotationY(toRadApp(90));
	matRotVertical.rotationX(toRadApp(-20)); 		// Zum spielen auf -10 setzen
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

void Application::createShadowTestScene(){
	pModel = new Model(ASSET_DIRECTORY "shadowcube.obj", false);
	pModel->shader(new PhongShader(), true);
	models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "bunny/bunny.dae", false);
	pModel->shader(new PhongShader(), true);
	models.push_back(pModel);
	
	// Directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0, -1, -1));
	dl->color(Color(0.5, 0.5, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	SpotLight* sl = new SpotLight();
	sl->position(Vector(2, 2, 0));
	sl->color(Color(0.5, 0.5, 0.5));
	sl->direction(Vector(-1, -1, 0));
	sl->innerRadius(10);
	sl->outerRadius(13);
	sl->castShadows(true);
	ShaderLightMapper::instance().addLight(sl);
}

void Application::createScene() {
	Matrix m;
	
	//------------------------------ LIGHTS ------------------------------
//	 Color c = Color(1.0f, 0.7f, 1.0f);
//	 Vector a = Vector(1, 0, 0.1f);
//	 float innerradius = 45;
//	 float outerradius = 60;
//
//	 // Directional lights
//	 DirectionalLight* dl = new DirectionalLight();
//	 dl->direction(Vector(0.2f, -1, 1));
//	 dl->color(Color(0.55, 0.55, 0.55));
//	 dl->castShadows(true);
//	 ShaderLightMapper::instance().addLight(dl);
//	 // Point lights
	 
	 // Point lights
//	 PointLight* pl = new PointLight();
//	 pl->position(Vector(-1.5, 3, 10));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
//
//	 pl = new PointLight();
//	 pl->position(Vector(15.0f, 3, 10));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
//
//
	 // Spot lights
//	 SpotLight* sl = new SpotLight();
//	 sl->position(Vector(-1.5, 3, 10));
//	 sl->color(c);
//	 sl->direction(Vector(1,-4,0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
//
//	 sl = new SpotLight();
//	 sl->position(Vector(5.0f, 3, 10));
//	 sl->color(c);
//	 sl->direction(Vector(-1, -4, 0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
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
		if(game.getCollisionHandler()->collisionDetection(pCharacter, *it, 0.001)) {
			std::cout << "collision with barrier" << std::endl;
			coolDownTimer = 10;
			game.getCollisionHandler()->handleCollisionWithBarrier(pCharacter, *it, 0.001, game.getControl());
			break;
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
		Matrix trans;
		
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(game.getCollisionHandler()->collisionDetection(pCharacter, (*it), DELTA) && (*it)->isCollected() == false) {
			game.foundCoin();
			coolDownTimer = 5; //Timer neu setzen
			std::cout << "found coin " << game.getCollectedCoins() << std::endl;
			game.getCollisionHandler()->handleCollisionWithCoin(*it);
		}
		
		if((*it)->isCollected() &&  pCoinMat->translation().Y > -6.0f) {
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
			std::cout << "collision with palette" << std::endl;
			coolDownTimer = 10;
			game.getCollisionHandler()->handleCollisionWithPalette(pCharacter, *it, 0.01, game.getControl());
		}
		else if(pCharacter->getPallet() == *it)
		{
			std::cout << "Fall" <<std::endl;
			pCharacter->setHovering(false);
			pCharacter->setPallet(NULL);
		}
	}
}

// Berechnung eines 3D-Strahls aus 2D-Mauskoordinaten
// Input: 	Fenster-Pixelkoordinaten des Mauszeigers, Ray Origin
// Output:	Ray Direction
/*
 Vector Application::calc3DRay( float x, float y, Vector& Pos){
 // 1. Normalisieren zwischen (-1,1)
 int windowWidth, windowHeight;
 glfwGetWindowSize(this->pWindow, &windowWidth, &windowHeight);
 
 float xNormal = 2.0f * x / (float) windowWidth - 1.0f;
 float yNormal = 1.0f - 2.0f * y / (float) windowHeight;
 
 // 2. Richtungsvektor in KAMERA-KOORDINATE erzeugen
 // (Projektionsmatrix invers auf normalisierte Koordinaten anwenden)
 Vector direction(xNormal, yNormal, 0);
 Matrix projection = Cam.getProjectionMatrix();
 direction = projection.invert() * direction;
 direction.normalize();
 
 // 3. Umrechnung von Kamera- zu Weltkoordinaten (Richtung anpassen)
 // Ursprung des Strahls ist Kameraposition (aus Cam.getViewMatrix())
 Matrix view = Cam.getViewMatrix();
 view.invert();
 
 Pos = view.translation(); //translation() gibt Vector(m03, m13, m23) zurück
 direction = view.transformVec3x3(direction);
 
 // 4. Schnittpunkt mit der Ebene Y=0 berechnen (Raytracing-Verfahren)
 Vector ny(0,1,0), y0(0,0,0);
 float s = (ny.dot(y0) - ny.dot(Pos)) / ny.dot(direction);
 
 return Pos + (direction * s);
 }
 */
