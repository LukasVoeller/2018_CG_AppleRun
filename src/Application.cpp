
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

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), time(0), Egocam(pWin), pModel(NULL), ShadowGenerator(2048, 2048){
	// Create LineGrid Model with constant Color Shader
	BaseModel* pModel = new LinePlaneModel(10, 10, 10, 10);
	ConstantShader* pCShader = new ConstantShader();
	pCShader->color(Color(1,1,1));
	pModel->shader(pCShader, true);
	LineGrid = pModel;
	
	createScene();
	//createNormalTestScene();
	//createShadowTestScene();
	
    ConstantShader* pConstShader = new ConstantShader();
    PhongShader* pPhongShader = new PhongShader();
	Matrix m,s;
	
	// CREATE GUI
	GUIEvents gui = GUIEvents();
	
	
	//------------------------------ MODELS ------------------------------
	// Tank
	pTank = new Tank();
	pTank->shader(pPhongShader, true);
	pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
	//m = m.translation(0, 0, 0);
	//pTank->transform(m);
	Models.push_back(pTank);
	
	//Obstacles
	for(int i=0; i<5; ++i)
	{
		float scaling = 2.0f;
		pBarrier1 = new Model(ASSET_DIRECTORY "buddha.dae", false, scaling);
		pBarrier1->shader(new PhongShader(), false);
		m = m.translation(5*i+4, 0, -5);
		s = s.scale(scaling);
		pBarrier1->transform(m*s);
		pBarriers.push_back(pBarrier1);
		Models.push_back(pBarrier1);
	}
	
	// Coins
	for(int i=0; i<5; ++i){
		coin = new Model(ASSET_DIRECTORY "buddha.dae", false);
		coin->shader(new PhongShader(), false);
		m = m.translation(5*i+2, 0, 5);
		s = s.scale(1);
		coin->transform(m*s);
		pCoins.push_back(coin);
		Models.push_back(coin);
	}
	
	// EgoCam
	Egocam.ProjMatrix().perspective(toRadApp(90), 4.0f/3.0f, 0.1f, 100.0f);
	Egocam.ViewMatrix().identity();
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

// For EgoCam
void Application::getInputPitchRollForward(float& pitch, float& roll, float& forward){
	pitch = 0;
	roll = 0;
	forward = 0;
	
	bool upPressed = glfwGetKey(pWindow, GLFW_KEY_UP ) == GLFW_PRESS;
	bool downPressed = (glfwGetKey(pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS || (glfwGetKey(pWindow, GLFW_KEY_Z ) == GLFW_PRESS));
	bool leftPressed = glfwGetKey(pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS;
	bool rightPressed = glfwGetKey(pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS;
	bool forwardPressed = glfwGetKey(pWindow, GLFW_KEY_W ) == GLFW_PRESS;
	bool backwardPressed = glfwGetKey(pWindow, GLFW_KEY_S ) == GLFW_PRESS;
	
	if(upPressed)
		pitch = 1;
	else if(downPressed)
		pitch = -1;
	
	if(leftPressed)
		roll = -1;
	else if(rightPressed)
		roll = 1;
	
	if(forwardPressed)
		forward = 1;
	else if(backwardPressed)
		forward = -1;
}

void Application::update(float dtime){
	Cam.update();
	time+=dtime;
	
	gui.update(pWindow, &Cam);
//	if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//		std::cout << "ESC " << gui.changeHelpMenu() << std::endl;
//
//	}
	
	// Tank steering
    double deltaTime = calcDeltaTime();
    float forwardBackward = getForwardBackward();
    float leftRight = getLeftRight();
    
    // Jump
    getJump();
    pTank->steer3d(forwardBackward, leftRight, this->downForce);
    if(pTank->getLatestPosition().Y < this->terrainHeight){
        pTank->setIsInAir(false);
        this->downForce = 0.0f;
	} else {
        this->downForce += gravity * 0.1f;
	}
	
	// Collision
	for(ModelList::iterator it = pBarriers.begin(); it != pBarriers.end(); ++it){
		if(collisionDetection(pTank, (Model*)(*it))){
			std::cout << "collision!" << std::endl;
			pTank->steer(-1 * forwardBackward, -1 * leftRight);
		}
	}
	
	// Aiming
	/*
	 double xpos, ypos;
	 glfwGetCursorPos(pWindow, &xpos, &ypos);
	 Vector pos = calc3DRay(xpos, ypos, pos);
	 pTank->aim(pos);
	 */
	
	// Tank steering
	Matrix tankMat = pTank->transform();
	float roll, pitch, forward;
	getInputPitchRollForward(pitch, roll, forward);
	
	Matrix rollMat, pitchMat, forwardMat;
	pitchMat.rotationX(pitch * dtime * 2.0f);
	rollMat.rotationZ(roll * dtime * 2.0f);
	forwardMat.translation(0, 0, forward * dtime * 2.0f);
	tankMat = tankMat * forwardMat * pitchMat * rollMat;
	pTank->transform(tankMat);
	
	// Version 1: Third person cam based on inverted object matrix
	Matrix matRot180;
	Matrix matTransView;
	matTransView.translation(0, 0.5f, 1);
	matRot180.rotationY(toRadApp(180));
	Matrix tankViewMatrix = tankMat * matRot180 * matTransView;
	tankViewMatrix.invert();
	Egocam.ViewMatrix() = tankViewMatrix;
	
	//pTank->update(deltaTime);
	
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
	ShadowGenerator.generate(Models);
	
    // Clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderLightMapper::instance().activate();
	
	// For EgoCam
	LineGrid->draw(Cam);
	
    // Setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it ){
        (*it)->draw(Cam);
    }
	
	//GUI
	gui.draw(&Cam);
	
	// For EgoCam
	Debug.render(Cam);
	
	ShaderLightMapper::instance().deactivate();
	
	// Third Person view
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glScissor(vp[0], vp[1], vp[2]/3, vp[3]/3);
	glViewport(vp[0], vp[1], vp[2]/3, vp[3]/3);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LineGrid->draw(Egocam);
	for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it ) {
		(*it)->draw(Egocam);
	}
	glScissor(vp[0], vp[1], vp[2], vp[3]);
	glViewport(vp[0], vp[1], vp[2], vp[3]);
	
    // Check once per frame for OpenGL errors
    GLenum Error = glGetError();
    assert(Error==0);
}

void Application::end(){
	for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it ){
        delete *it;
	}
    Models.clear();
}

void Application::createScene(){
	Matrix m;
	
	//------------------------------ SCENE ------------------------------
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "base.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(30, 0, 0);
	pModel->transform(m);
	Models.push_back(pModel);

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

void Application::createNormalTestScene(){
	pModel = new LinePlaneModel(10, 10, 10, 10);
	ConstantShader* pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 0, 0));
	pModel->shader(pConstShader, true);
    
	// Add to render list
	Models.push_back(pModel);
	pModel = new Model(ASSET_DIRECTORY "cube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
}

void Application::createShadowTestScene(){
	pModel = new Model(ASSET_DIRECTORY "shadowcube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "bunny.dae", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
	
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

float Application::getLeftRight(){
    float direction = 0.0f;
	
    // Strafe right
	if (glfwGetKey(pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        direction -= 2.0f;
	}
	
    // Strafe left
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
        direction += 2.0f;
	}
	std::cout << "getLeftRight " << direction  << std::endl;
    return direction;
}

float Application::getForwardBackward(){
    float direction = 0.0f;
	
    // Move forward
	if (glfwGetKey(pWindow, GLFW_KEY_UP ) == GLFW_PRESS){
        direction += 2.0f;
	}
	
    // Move backward
	if (glfwGetKey(pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
        direction -= 2.0f;
	}
	std::cout << "getForwardBackward" << direction << std::endl;
    return direction;
}

void Application::getJump(){
    if(!pTank->getIsInAir()){
		if (glfwGetKey(pWindow, GLFW_KEY_SPACE ) == GLFW_PRESS){
            pTank->setIsInAir(true);
            this->downForce = pTank->getJumpPower();
        }
    }
	std::cout << "getJump" << std::endl;
}

bool Application::collisionDetection(Tank* model1, Model* model2)
{
	
//	const AABB& box1 = model1->boundingBox();
//	const AABB& box2 = model2->boundingBox();
//
////	//Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
////	return (box1.Min.X < box2.Max.X &&
////			box2.Min.X < box1.Max.X &&
////			box1.Min.Z < box2.Max.Z &&
////			box2.Min.Z < box1.Max.Z);
	
	Vector vec1 = model1->transform().translation();
    Vector vec2 = model2->transform().translation();

	Vector size1 = model1->boundingBox().size();
    Vector size2 = model2->boundingBox().size();

    //Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
    return (vec1.X - size1.X/2 < vec2.X + size2.X/2 &&
    	vec2.X - size2.X/2 < vec1.X + size1.X/2 &&
		vec1.Y - size1.Y/2 < vec2.Y + size2.Y/2 &&
		vec2.Y - size2.Y/2 < vec1.Y + size1.Y/2 &&
    	vec1.Z - size1.Z/2 < vec2.Z + size2.Z/2 &&
    	vec2.Z - size2.Z/2 < vec1.Z + size1.Z/2);

}
