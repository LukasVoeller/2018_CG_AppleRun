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

Application::Application(GLFWwindow* pWin) : pWindow(pWin), time(0), egocam(pWin), pModel(NULL), shadowGenerator(2048, 2048){
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
	//pScene->addSceneFile(ASSET_DIRECTORY "scenemodel.osh");
	pScene->addSceneFile(ASSET_DIRECTORY "testscene.osh");
	models.push_back(pScene);
	
	pBarriers = pScene->getObstacles();
	pCoins = pScene->getCoins();
	pDeathblocks = pScene->getDeathItems();

	//createScene();
	//createNormalTestScene();
	//createShadowTestScene();
	//glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//OutlineShader* pOutlineShader = new OutlineShader();
	
	// Create GUI
	GUIEvents gui = GUIEvents();
	
	// Camera
	egocam.ViewMatrix().identity();
	egocam.ProjMatrix().perspective((float)M_PI*65.0f/180.0f, 640/480, 0.045f, 1000.0f);
	//Egocam.ProjMatrix().perspective(toRadApp(90), 4.0f/3.0f, 0.1f, 100.0f);
	
	//------------------------------ MODELS ------------------------------
	Matrix m,s,r;
	
	// Tank
	pTank = new Tank();
	pPhongShader = new PhongShader();
	pTank->shader(pPhongShader, true);
	pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
	m = m.translation(START_POS_X, START_POS_Y, START_POS_Z);
	pTank->transform(m);
	
	models.push_back(pTank);
	
//	float baymaxScaling = 0.2;
//	pTest = new Model(ASSET_DIRECTORY "BaymaxWhiteOBJ/Bigmax_White_OBJ.obj", false, baymaxScaling);
//	//pTest->shader(pPhongShader, false);
//	pTest->shader(pOutlineShader, false);
//	s = s.scale(baymaxScaling);
//	m = m.translation(-4, 0, -4);
//	pTest->transform(m*s);
//	Models.push_back(pTest);

	
	//------------------------------ GAME LOGIC ------------------------------
	allCoins = ALLCOINS;
	//allCoins = (unsigned int) pCoins.size();
	collectedCoins = 0;
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

// Andere Bedienung
void Application::getInputPitchRollForward(float& pitch, float& roll, float& forward){
	pitch = 0;
	roll = 0;
	forward = 0;
	
	bool upPressed = glfwGetKey(pWindow, GLFW_KEY_UP ) == GLFW_PRESS;
	bool downPressed = (glfwGetKey(pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS ||
						(glfwGetKey(pWindow, GLFW_KEY_Z ) == GLFW_PRESS));
	bool leftPressed = glfwGetKey(pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS;
	bool rightPressed = glfwGetKey(pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS;
	bool forwardPressed = glfwGetKey(pWindow, GLFW_KEY_W ) == GLFW_PRESS;
	bool backwardPressed = glfwGetKey(pWindow, GLFW_KEY_S ) == GLFW_PRESS;
	
	if(upPressed)
		pitch = 0;
	else if(downPressed)
		pitch = 0;
	
	if(leftPressed)
		roll = 0;
	else if(rightPressed)
		roll = 0;
	
	if(forwardPressed)
		forward = 1;
	else if(backwardPressed)
		forward = -1;
}

void Application::update(float dtime){
	egocam.update();
	gui.update(pWindow, &egocam);
	
	time+=dtime;
	
	// Tank steering
    double deltaTime = calcDeltaTime();
    float forwardBackward = getForwardBackward();
    float leftRight = getLeftRight();
    
    // Jump
	// Blöd, weil man von Blöcken nicht weiterspringen kann...
    getJump();
    pTank->steer3d(forwardBackward, leftRight, this->downForce);
    if(pTank->getLatestPosition().Y <= this->terrainHeight + DELTA){
        pTank->setIsInAir(false);
        this->downForce = 0.0f;
	} else {
        this->downForce += gravity * 0.1f;
		std::cout << "DownForce " << downForce << std::endl;
	}

	// Collision
	int count =0;
	for(NodeList::iterator it = pBarriers.begin(); it != pBarriers.end(); ++it){
		//std::cout << "Barrier " << ++count << std::endl;
		//gehört hier eigt nicht hin
		(*it)->getModel()->transform((*it)->getLocalTransform());
		
		if (coolDownTimer > 0) {
			coolDownTimer--;
		}
		else {
			if(collisionDetection(pTank, *it)){
				std::cout << "collision with barrier" << std::endl;
                coolDownTimer = 10;
                
                if(pTank->getLatestPosition().Y > terrainHeight + DELTA ) {
                    std::cout << "Oben "<< pTank->getLatestPosition().Y << std::endl;
                    this->downForce = 0.0f;
                    pTank->setIsInAir(false);
                    
                }
                else {
                    std::cout << "Daneben "<< std::endl;
                    pTank->steer3d(-10 * forwardBackward * deltaTime, -10 * leftRight * deltaTime, 0);
                }
				
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
		}
		else {
			if(collisionDetection(pTank, *it)){
				std::cout << "death!" << std::endl;
				coolDownTimer = 10;
				reset(deltaTime);
				//auf start zurücksetzen
			}
		}
	}
	
	count = 0;
	for(NodeList::iterator it = pCoins.begin(); it != pCoins.end(); ++it){
		std::cout << "Coin " << ++count << " "<< (*it)->isCollected() <<  std::endl;
        (*it)->getGlobalTransform().translation().debugOutput();
		const Matrix* pCoinMat = &(*it)->getLocalTransform();
		
		Matrix trans;
		//Besser iwo anders hin damit... eigentlich hier falsch
		if(!(*it)->isCollected()) {
			(*it)->getModel()->transform((*it)->getLocalTransform());

		}

		if (coolDownTimer > 0) {
			coolDownTimer--;
		}
		else if(collisionDetection(pTank, (*it)) && coolDownTimer == 0 && (*it)->isCollected() == false){
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
	
	// Aiming
	//double xpos, ypos;
	//glfwGetCursorPos(pWindow, &xpos, &ypos);
	//Vector pos = calc3DRay(xpos, ypos, pos);
	//pTank->aim(pos);
	
	// Tank steering
//	float roll, pitch, forward;
	Matrix tankMat = pTank->transform();
//	getInputPitchRollForward(pitch, roll, forward);
//
//	Matrix rollMat, pitchMat, forwardMat;
//	pitchMat.rotationX(pitch * dtime * 2.0f);
//	rollMat.rotationZ(roll * dtime * 2.0f);
//	forwardMat.translation(0, 0, forward * dtime * 2.0f);
//	tankMat = tankMat * forwardMat * pitchMat * rollMat;
//	pTank->transform(tankMat);

	// Version 1: Third person cam based on inverted object matrix
	Matrix matRotHorizontal;
	Matrix matRotVertical;
	Matrix matTransView;
	matTransView.translation(0, 2.0f, 5);
	matRotHorizontal.rotationY(toRadApp(-90));
	matRotVertical.rotationX(toRadApp(-30));
	Matrix tankViewMatrix = tankMat * matRotHorizontal * matRotVertical * matTransView;
	tankViewMatrix.invert();
	egocam.ViewMatrix() = tankViewMatrix;
	 
	// Version 2: Third person cam based on lookAt matrix
	/*
	Matrix tankViewMat;
	tankViewMat.lookAt(tankMat.translation() + tankMat.forward(), tankMat.up(), tankMat.translation() + tankMat.up() * 0.5f - tankMat.forward());
	Matrix tmp = tankViewMat;
	tmp.invert();
	Debug.drawMatrix(tmp);
	Egocam.ViewMatrix() = tankViewMat;
	 */
	
	// Version 3: Interpolated third person cam
	/*
	Matrix matRot180;
	Matrix matTransView;
	matTransView.translation(0, 0.5f, 1);
	matRot180.rotationY(toRadApp(180));
	Matrix matTank = tankMat * matRot180 * matTransView;
	
	Matrix viewMat = Egocam.ViewMatrix();
	viewMat.invert();
	
	Vector p = viewMat.translation().lerp(matTank.translation(), 0.1f);
	Vector f = viewMat.forward().lerp(matTank.forward(), 0.01f);
	Vector u = viewMat.up().lerp(matTank.up(), 0.01f);
	f.normalize();
	u.normalize();
	
	Vector r = u.cross(f);
	r.normalize();
	u = f.cross(r);
	
	viewMat.translation(p);
	viewMat.up(u);
	viewMat.forward(f);
	viewMat.right(r);
	Debug.drawMatrix(viewMat);
	viewMat.invert();
	Egocam.ViewMatrix() = viewMat;
	 */
	
	pTank->update(deltaTime);
}

// Elapsed time since last call of the method
double Application::calcDeltaTime(){
    double now = glfwGetTime();
    double deltaTime = (now - this->oldTime);
    this->oldTime = now;
    if (this->oldTime == 0){
        return 1/60;	// 1/60 = 60 frames per second
    }
    return deltaTime;
}

void Application::draw(){
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
	//shadowGenerator.generate(models);
	//ShaderLightMapper::instance().activate();
	//ShaderLightMapper::instance().deactivate();
	
    // Check once per frame for OpenGL errors
    GLenum error = glGetError();
    assert(error==0);
}

void Application::end(){
	for( ModelList::iterator it = models.begin(); it != models.end(); ++it ){
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

	pModel = new Model(ASSET_DIRECTORY "bunny.dae", false);
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

float Application::getLeftRight(){
    float direction = 0.0f;
    // Strafe right
	if (glfwGetKey(pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		direction -= ROTATIONSPEED;
	}
    // Strafe left
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
        direction += ROTATIONSPEED;
	}
//	std::cout << "getLeftRight " << direction  << std::endl;
    return direction;
}

float Application::getForwardBackward()
{
	float direction = 0.0f;
//	float speed = 0.0f;
	float speed = (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS) ? ADDSPEED : 0.0f;
	
    // Move forward
	if (glfwGetKey(pWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		speed += RUNSPEED;
        direction += speed;
	}
    // Move backward
	if (glfwGetKey(pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		speed += RUNSPEED;
		direction -= speed;
	}
    return direction;
}

void Application::getJump(){
    if(!pTank->getIsInAir()){
		if (glfwGetKey(pWindow, GLFW_KEY_SPACE ) == GLFW_PRESS){
            pTank->setIsInAir(true);
            this->downForce = pTank->getJumpPower();
        }
    }
}

/******* Old collisionDetection for things not in the scene **/
bool Application::collisionDetection(Tank* model1, Model* model2)
{
	Vector vec1 = model1->transform().translation();
    Vector vec2 = model2->transform().translation();
	
	Vector size1 = model1->getBoundingBox().size();
	Vector size2 = model2->getScaledBoundingBox().size();

    //Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
    return (vec1.X - size1.X/2 < vec2.X + size2.X/2 &&
    	vec2.X - size2.X/2 < vec1.X + size1.X/2 &&
		vec1.Y - size1.Y/2 < vec2.Y + size2.Y/2 &&
		vec2.Y - size2.Y/2 < vec1.Y + size1.Y/2 &&
    	vec1.Z - size1.Z/2 < vec2.Z + size2.Z/2 &&
    	vec2.Z - size2.Z/2 < vec1.Z + size1.Z/2);
}

/****** Collision with scenenode *********/
bool Application::collisionDetection(Tank* model1, SceneNode* node)
{
	Vector vec1 = model1->transform().translation();
	Vector vec2 = node->getLocalTransform().translation();
	
	Vector size1 = model1->getBoundingBox().size();
	Vector size2 = node->getScaledBoundingBox().size();
	
	//Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
	return (vec1.X - size1.X/2 < vec2.X + size2.X/2 &&
			vec2.X - size2.X/2 < vec1.X + size1.X/2 &&
			vec1.Y - size1.Y/2 < vec2.Y + size2.Y/2 &&
			vec2.Y - size2.Y/2 < vec1.Y + size1.Y/2 &&
			vec1.Z - size1.Z/2 < vec2.Z + size2.Z/2 &&
			vec2.Z - size2.Z/2 < vec1.Z + size1.Z/2);
}

void Application::createScene(){
	Matrix m;
	
	//------------------------------ SCENE ------------------------------
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	models.push_back(pModel);
	
	pModel = new Model(ASSET_DIRECTORY "base.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(30, 0, 0);
	pModel->transform(m);
	models.push_back(pModel);
	
	//------------------------------ LIGHTS ------------------------------
	/*
	 Color c = Color(1.0f, 0.7f, 1.0f);
	 Vector a = Vector(1, 0, 0.1f);
	 float innerradius = 45;
	 float outerradius = 60;
	 
	 // Directional lights
	 DirectionalLight* dl = new DirectionalLight();
	 dl->direction(Vector(0.2f, -1, 1));
	 dl->color(Color(0.25, 0.25, 0.5));
	 dl->castShadows(true);
	 ShaderLightMapper::instance().addLight(dl);
	 
	 // Point lights
	 PointLight* pl = new PointLight();
	 pl->position(Vector(-1.5, 3, 10));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 pl = new PointLight();
	 pl->position(Vector(5.0f, 3, 10));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 pl = new PointLight();
	 pl->position(Vector(-1.5, 3, 28));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 pl = new PointLight();
	 pl->position(Vector(5.0f, 3, 28));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 pl = new PointLight();
	 pl->position(Vector(-1.5, 3, -8));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 pl = new PointLight();
	 pl->position(Vector(5.0f, 3, -8));
	 pl->color(c);
	 pl->attenuation(a);
	 ShaderLightMapper::instance().addLight(pl);
	 
	 // Spot lights
	 SpotLight* sl = new SpotLight();
	 sl->position(Vector(-1.5, 3, 10));
	 sl->color(c);
	 sl->direction(Vector(1,-4,0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 
	 sl = new SpotLight();
	 sl->position(Vector(5.0f, 3, 10));
	 sl->color(c);
	 sl->direction(Vector(-1, -4, 0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 
	 sl = new SpotLight();
	 sl->position(Vector(-1.5, 3, 28));
	 sl->color(c);
	 sl->direction(Vector(1, -4, 0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 
	 sl = new SpotLight();
	 sl->position(Vector(5.0f, 3, 28));
	 sl->color(c);
	 sl->direction(Vector(-1, -4, 0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 
	 sl = new SpotLight();
	 sl->position(Vector(-1.5, 3, -8));
	 sl->color(c);
	 sl->direction(Vector(1, -4, 0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 
	 sl = new SpotLight();
	 sl->position(Vector(5.0f, 3, -8));
	 sl->color(c);
	 sl->direction(Vector(-1, -4, 0));
	 sl->innerRadius(innerradius);
	 sl->outerRadius(outerradius);
	 ShaderLightMapper::instance().addLight(sl);
	 */
}

void Application::reset(float dtime) {
	gui.restartGame(); //Startbildschirm aufrufen
	collectedCoins = 0;
	
	// alle gesammelten Coins wieder positionieren
	for(NodeList::iterator it = pCoins.begin(); it != pCoins.end(); ++it){
		if((*it)->isCollected()) {
			(*it)->setCollected(false);
			std::cout << "reset" << std::endl;
			Matrix t;
			
			(*it)->setLocalTransform(Vector((*it)->getLatestPosition().X, 0, (*it)->getLatestPosition().Z), Vector(0, 1, 0), 0);
		}
	}
	//Figur wieder auf den Startpunkt
	Matrix m;
	m = m.translation(START_POS_X, START_POS_Y, START_POS_Z);
	pTank->transform(m);
}
