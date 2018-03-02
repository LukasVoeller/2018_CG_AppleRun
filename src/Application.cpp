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

float toRadApp(float deg){ return deg*M_PI/180.0f; }

Application::Application(GLFWwindow* pWin) : pWindow(pWin), time(0), egocam(pWin), pModel(NULL), shadowGenerator(2048, 2048) {
	BaseModel* pModel;
	ConstantShader* pConstShader;
	PhongShader* pPhongShader = new PhongShader();
	
	// Create lineGrid model with constant color shader
	pModel = new LinePlaneModel(10, 10, 10, 10);
	pConstShader = new ConstantShader();
	pConstShader->color( Color(1,1,1));
	pModel->shader(pConstShader, true);

	models.push_back( pModel );
	lineGrid = pModel;

	pScene = new Scene();
	pScene->shader(new PhongShader(), true);
	pScene->addSceneFile(ASSET_DIRECTORY "scene.osh");
	models.push_back(pScene);

	pBarriers = pScene->getObstacles();
	pCoins = pScene->getCoins();
	pDeathblocks = pScene->getDeathItems();
	pMovingItems = pScene->getMovingItems();

	//createScene();
	//createNormalTestScene();
	//createShadowTestScene();
	
	//OutlineShader* pOutlineShader = new OutlineShader();
	
	// Create GUI
	GUIEvents gui = GUIEvents();
	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Camera
	egocam.ViewMatrix().identity();
	egocam.ProjMatrix().perspective((float)M_PI*65.0f/180.0f, 640/480, 0.045f, 1000.0f);
	//Egocam.ProjMatrix().perspective(toRadApp(90), 4.0f/3.0f, 0.1f, 100.0f);
	
	//------------------------------ MODELS ------------------------------
	Matrix m,s,r;
	
	// Robot
	pCharacter = new Character();
	float scaling = 1.0f;
	pPhongShader = new PhongShader();
	pCharacter->shader(pPhongShader, true);
	pCharacter->loadModel(ASSET_DIRECTORY "android/Android.dae", scaling);
	m = m.translation(START_POS_X, START_POS_Y, START_POS_Z);
	s = s.scale(scaling);
	pCharacter->transform(m*s);
	
	models.push_back(pCharacter);
	
	//------------------------------ GAME LOGIC ------------------------------
	allCoins = ALLCOINS;
	//allCoins = (unsigned int) pCoins.size();
	collectedCoins = 0;
	
	//GameLogic
	game = Game();
	
	//Character-Control
	playerControl = Control(pWin);
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
	egocam.update();
	gui.update(pWindow, &egocam);
	plattformsHover();
	
	characterViewMatrix = calcCharacterViewMatrix(pCharacter);
	egocam.ViewMatrix() = characterViewMatrix;
	
	time+=dtime;
	
	for(MovingItemList::iterator it = pMovingItems.begin(); it != pMovingItems.end(); ++it) {
		if (coolDownTimer > 0) {
			coolDownTimer--;
			continue;
		}
		if(collisionDetection(pCharacter, *it)) {
			std::cout << "collision with palette" << std::endl;
			coolDownTimer = 10;
			palletCollisionHandling(pCharacter, *it);
		}
		else if(pCharacter->getPallet() == *it)
		{
			//no collision
			std::cout << "Fall" <<std::endl;
			pCharacter->setHovering(false);
			pCharacter->setPallet(NULL);
		}
	}
	
	// Character steering
    deltaTime = calcDeltaTime();
    forwardBackward = playerControl.readForwardBackward();
    leftRight = playerControl.readLeftRight();
	downForce = playerControl.readJump(this->pCharacter);
	pCharacter->steer3d(forwardBackward, leftRight, downForce);
	pCharacter->setPosZ(0.0f);
	playerControl.handleJump(pCharacter);
	
	// Collision
	int count =0;
	for(NodeList::iterator it = pBarriers.begin(); it != pBarriers.end(); ++it){
		//std::cout << "Barrier " << ++count << std::endl;
		//gehört hier eigt nicht hin
		(*it)->getModel()->transform((*it)->getLocalTransform());
		
		if (coolDownTimer > 0) {
			coolDownTimer--;
		} else {
			if(collisionDetection(pCharacter, *it)){
				std::cout << "collision with barrier" << std::endl;
                coolDownTimer = 10;
				collisionHandling(pCharacter, *it);
			}
		}
	}

	// Collision
	count = 0;
	for(NodeList::iterator it = pDeathblocks.begin(); it != pDeathblocks.end(); ++it){
		//Muss eigt woanders hin?
		(*it)->getModel()->transform((*it)->getLocalTransform());

		if (coolDownTimer > 0) {
			coolDownTimer--;
		} else {
			if(collisionDetection(pCharacter, *it)){
				std::cout << "You died!" << std::endl;
				coolDownTimer = 10;
				reset(deltaTime);
				//auf start zurücksetzen
			}
		}
	}
	
	count = 0;
	for(CoinList::iterator it = pCoins.begin(); it != pCoins.end(); ++it){
		//std::cout << "Coin " << ++count << " "<< (*it)->isCollected() <<  std::endl;
		const Matrix* pCoinMat = &(*it)->getLocalTransform();
		
		Matrix trans;
		//Besser iwo anders hin damit... eigentlich hier falsch
		if(!(*it)->isCollected()) {
			(*it)->getModel()->transform((*it)->getLocalTransform());
		}

		if (coolDownTimer > 0) {
			coolDownTimer--;
		} else if(collisionDetection(pCharacter, (*it)) && coolDownTimer == 0 && (*it)->isCollected() == false) {
			collectedCoins++;
			coolDownTimer = 5; //Timer neu setzen
			std::cout << "found coin " << collectedCoins << std::endl;
			(*it)->setCollected(true);

			trans.translation(0, 2.5f, 0);
			(*it)->setLocalTransform((*it)->getLocalTransform()*trans);
		}
		
		if((*it)->isCollected() &&  pCoinMat->translation().Y > -6.0f) {
			Matrix t;
			std::cout << "update coin" << (*it)->getLocalTransform().translation().Y << std::endl;
			float newHeight = pCoinMat->translation().Y - 0.25f;

			t.translation(pCoinMat->translation().X, newHeight, pCoinMat->translation().Z);
			(*it)->setLocalTransform(t);
			(*it)->setLatestPosition(pCoinMat->translation());
		}
	}
	
	if(collectedCoins == allCoins) {
		gui.wonGame();
	}
	
	pCharacter->update(deltaTime);
}

// Third person cam based on inverted object matrix
Matrix Application::calcCharacterViewMatrix(Character* character) {
	Matrix characterMat = character->transform();
	Matrix matRotHorizontal, matRotVertical, matTransView;
	matTransView.translation(0, 5, 15);
	matRotHorizontal.rotationY(toRadApp(-90));
	matRotVertical.rotationX(toRadApp(-20));
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

void Application::createNormalTestScene(){
	pModel = new LinePlaneModel(10, 10, 10, 10);
	ConstantShader* pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 0, 0));
	pModel->shader(pConstShader, true);
    
	// Add to render list
	models.push_back(pModel);
	pModel = new Model(ASSET_DIRECTORY "cube.obj", false);
	pModel->shader(new PhongShader(), true);
	models.push_back(pModel);
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

/****** Kollision mit Scenenode *********/
/* DELTA für Sicherheitsabstand **********/
//IDEE: nehme nicht Position und größe, sondern Position und Boundingbox.min bzw. max...
bool Application::collisionDetection(Character* model1, SceneNode* node) {
	Vector vec1 = model1->transform().translation();
	Vector vec2 = node->getLocalTransform().translation();
	
	AABB bb1 = model1->getScaledBoundingBox();
	AABB bb2 = node->getScaledBoundingBox();
	
	//Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
	return (vec1.X + bb1.Min.X + DELTA < vec2.X + bb2.Max.X &&
			vec2.X + bb2.Min.X + DELTA < vec1.X + bb1.Max.X &&
			vec1.Y + bb1.Min.Y + DELTA < vec2.Y + bb2.Max.Y &&
			vec2.Y + bb2.Min.Y + DELTA < vec1.Y + bb1.Max.Y &&
			vec1.Z + bb1.Min.Z + DELTA < vec2.Z + bb2.Max.Z &&
			vec2.Z + bb2.Min.Z + DELTA < vec1.Z + bb1.Max.Z);
}

void Application::createScene() {
	Matrix m;
	
	//------------------------------ LIGHTS ------------------------------
	 Color c = Color(1.0f, 0.7f, 1.0f);
	 Vector a = Vector(1, 0, 0.1f);
	 float innerradius = 45;
	 float outerradius = 60;
	 
	 // Directional lights
//	 DirectionalLight* dl = new DirectionalLight();
//	 dl->direction(Vector(0.2f, -1, 1));
//	 dl->color(Color(0.55, 0.55, 0.55));
//	 dl->castShadows(true);
//	 ShaderLightMapper::instance().addLight(dl);
	 
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
//	 pl = new PointLight();
//	 pl->position(Vector(-1.5, 3, 28));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
//
//	 pl = new PointLight();
//	 pl->position(Vector(15.0f, 3, 28));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
//
//	 pl = new PointLight();
//	 pl->position(Vector(-1.5, 3, -8));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
//
//	 pl = new PointLight();
//	 pl->position(Vector(5.0f, 3, -8));
//	 pl->color(c);
//	 pl->attenuation(a);
//	 ShaderLightMapper::instance().addLight(pl);
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
//
//	 sl = new SpotLight();
//	 sl->position(Vector(-1.5, 3, 28));
//	 sl->color(c);
//	 sl->direction(Vector(1, -4, 0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
//
//	 sl = new SpotLight();
//	 sl->position(Vector(5.0f, 3, 28));
//	 sl->color(c);
//	 sl->direction(Vector(-1, -4, 0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
//
//	 sl = new SpotLight();
//	 sl->position(Vector(-1.5, 3, -8));
//	 sl->color(c);
//	 sl->direction(Vector(1, -4, 0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
//
//	 sl = new SpotLight();
//	 sl->position(Vector(5.0f, 3, -8));
//	 sl->color(c);
//	 sl->direction(Vector(-1, -4, 0));
//	 sl->innerRadius(innerradius);
//	 sl->outerRadius(outerradius);
//	 ShaderLightMapper::instance().addLight(sl);
}

void Application::reset(float dtime) {
	gui.restartGame(); //Startbildschirm aufrufen
	collectedCoins = 0;
	
	/*Setzt die Figur auf die Startposition zurück und positioniert alle Coins wieder auf die Startposition */
	game.start(pCharacter, pCoins);
}

void Application::plattformsHover() {
	for(MovingItemList::iterator it = pMovingItems.begin(); it != pMovingItems.end(); ++it){
		Vector t = (*it)->getLocalTransform().translation();		
		float heigth = t.Y;
		
		if (heigth > 13.0f) {
			(*it)->setMoveUp(false);
		}
		if(heigth < 0.8f) {
			(*it)->setMoveUp(true);
		}
		
		heigth = (*it)->isMovingUp() ? heigth + 0.05 : heigth - 0.05;
		
		Matrix trans;
		trans.translation(t.X, heigth, t.Z);
		(*it)->setLocalTransform(trans);
	}
}

/* Behandlung aller Fälle
 * a. von Oben auf eine Kiste springen
 * b. seitlich dagegen springen (also in der Luft)
 * c. seitlich dagegen fahren (auf dem Boden
 * d. von unten nach oben dagegen springen */
void Application::collisionHandling(Character* model1, SceneNode* model2) {
	Matrix t;
	Matrix m = model1->transform();
	
	Vector pos1 = model1->getLatestPosition();
	Vector pos2 = model2->getLocalTransform().translation();
	Vector size2 = model2->getScaledBoundingBox().size();
	float bMaxY = pos2.Y + 0.5f* model2->getScaledBoundingBox().size().Y;
	float bMinY = pos2.Y - 0.5f* model2->getScaledBoundingBox().size().Y;
	float cMinY = pos1.Y - 0.5f* model1->getScaledBoundingBox().size().Y;
	float cMaxY = pos1.Y + 0.5f* model1->getScaledBoundingBox().size().Y;
	
	if(pos1.Y <= TERRAIN_HEIGHT && !model1->getIsInAir()) {
		std::cout << "seite" << std::endl;
		t.translation(-6*forwardBackward*deltaTime, 0, -6*leftRight*deltaTime);
		model1->transform(m*t);
		return;
	}
	
	// Von oben bzw. in der oberen Hälfte
	else if(bMaxY > cMinY && cMinY > pos2.Y ) {
		std::cout << "oben" << std::endl;
		playerControl.setJumpPower(0.0f);
		pCharacter->setIsInAir(false);
		return;
	}
	
	// Von unten -> begrenze die Sprunghöhe
	else if(bMinY < cMaxY  && cMaxY < pos2.Y) {
		std::cout << "von unten" << std::endl;
		playerControl.setJumpPower(-3.0f);
	}
	
	// Auf der Erde
	else {
		std::cout << "seite..." << std::endl;
		t.translation(-6*forwardBackward*deltaTime, 0, -6*leftRight*deltaTime);
		model1->transform(m*t);
	}
}

void Application::palletCollisionHandling(Character* model1, MovingItem* model2) {
	Matrix t;
	Matrix m = model1->transform();
	
	Vector pos1 = model1->getLatestPosition();
	Vector pos2 = model2->getLocalTransform().translation();
	Vector size2 = model2->getScaledBoundingBox().size();
	float bMaxY = pos2.Y + 0.5f* model2->getScaledBoundingBox().size().Y;
	float bMinY = pos2.Y - 0.5f* model2->getScaledBoundingBox().size().Y;
	float cMinY = pos1.Y - 0.5f* model1->getScaledBoundingBox().size().Y;
	float cMaxY = pos1.Y + 0.5f* model1->getScaledBoundingBox().size().Y;
	
	if(pCharacter->getPallet() == NULL) {
		// Von oben bzw. in der oberen Hälfte
		if(bMaxY > cMinY && cMinY > pos2.Y) {
			std::cout << "oben" << std::endl;
			playerControl.setJumpPower(0.0f);
			pCharacter->setIsInAir(false);
			pCharacter->setHovering(true);
			pCharacter->setPallet(model2);
			return;
		}
		
		// Von unten -> begrenze die Sprunghöhe
		else if(bMinY < cMaxY  && cMaxY < pos2.Y) {
			std::cout << "von unten" << std::endl;
			playerControl.setJumpPower(-3.0f);
		}
		
		// Auf der Erde
		else {
			std::cout << "seite..." << std::endl;
			t.translation(-6*forwardBackward*deltaTime, 0, -6*leftRight*deltaTime);
			model1->transform(m*t);
		}
	}
	else {
		std::cout << "Flyyyy "<< pCharacter->getLatestPosition().Y << std::endl;
	}
}
