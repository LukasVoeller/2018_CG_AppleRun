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

#include "Application.h"
#include "LinePlaneModel.h"
#include "TrianglePlaneModel.h"
#include "TriangleSphereModel.h"
#include "LineBoxModel.h"
#include "TriangleBoxModel.h"
#include "Model.h"
#include "ShaderLightMapper.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048)
{
	createScene();
	//createNormalTestScene();
	//createShadowTestScene();
    BaseModel* pModel;
    ConstantShader* pConstShader;
    PhongShader* pPhongShader;
    
    pPhongShader = new PhongShader();
    
    pTank = new Tank();
    pTank->shader(pPhongShader, true);
    pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
    Models.push_back( pTank );
    
    pBarrier = new Model(ASSET_DIRECTORY "bunny.dae", false);
    pBarrier->shader(new PhongShader(), false);
    Matrix m;
    m = m.translation(0, 0, -5);
    pBarrier->transform(m);
    Models.push_back(pBarrier);
}

void Application::start()
{
    glEnable (GL_DEPTH_TEST);   // enable depth-testing
    glDepthFunc (GL_LESS);      // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime)
{
    double deltaTime = calcDeltaTime();
    float forwardBackward = getForwardBackward();
    float leftRight = getLeftRight();
    
    //Springen
    getJump();
    pTank->steer3d(forwardBackward, leftRight, this->downForce);
    if(pTank->getLatestPosition().Y < this->terrainHeight)
    {
        pTank->setIsInAir(false);
        this->downForce = 0.0f;
    } else
        this->downForce += gravity * 0.1f;

    pTank->printLatestPosition();

    bool collision = collisionDetection(pTank, pBarrier);
    //std::cout << "Kollision is " << collision << std::endl;
    
    if(collision)
        pTank->steer(-1*forwardBackward, -1*leftRight);
    
    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);
    Vector pos = calc3DRay(xpos, ypos, pos);
    pTank->aim(pos);
    
    pTank->update(deltaTime);
    Cam.update();
}

//Vergangene Zeit seit letztem Aufruf der Methode
double Application::calcDeltaTime() {
    double now = glfwGetTime();
    double deltaTime = (now - this->oldTime);
    this->oldTime = now;
    if (this->oldTime == 0) {
        return 1/60;    // 1/60 = 60 frames per second
    }
    return deltaTime;
}

void Application::draw()
{
	ShadowGenerator.generate(Models);
	
    // clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderLightMapper::instance().activate();
    
    // setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
	ShaderLightMapper::instance().deactivate();
	
    // check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}

void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

void Application::createScene()
{
	Matrix m,n;

	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "scene.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(10, 0, -10);
	pModel->transform(m);
	Models.push_back(pModel);
/*
    pModel = new Model(ASSET_DIRECTORY "scene.dae", false);
    pModel->shader(new PhongShader(), true);
    m.translation(10, 0, -10);
    pModel->transform(m);
    Models.push_back(pModel);
*/
	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	Color c = Color(1.0f, 0.7f, 1.0f);
	Vector a = Vector(1, 0, 0.1f);
	float innerradius = 45;
	float outerradius = 60;
	
	// point lights
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
	
	// spot lights
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
}

void Application::createNormalTestScene()
{
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

void Application::createShadowTestScene()
{
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
// Input: Fenster-Pixelkoordinaten des Mauszeigers
Vector Application::calc3DRay( float x, float y, Vector& Pos)
{
    // TODO:    Add your code here
    // Pos:     Ray Origin
    // Return:  Ray Direction
    
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

float Application::getLeftRight() {
    float direction = 0.0f;
    //Strafe right
    if ((glfwGetKey(pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS) || (glfwGetKey(pWindow, GLFW_KEY_D ) == GLFW_PRESS)){
        direction -= 3.0f;
    }
    //Strafe left
    if ((glfwGetKey(pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS) || (glfwGetKey(pWindow, GLFW_KEY_A ) == GLFW_PRESS)){
        direction += 3.0f;
    }
    
    return direction;
}

float Application::getForwardBackward() {
    float direction = 0.0f;
    //Move forward
    if ((glfwGetKey(pWindow, GLFW_KEY_UP ) == GLFW_PRESS) || (glfwGetKey(pWindow, GLFW_KEY_W ) == GLFW_PRESS)){
        direction += 3.0f;
    }
    //Move backward
    if ((glfwGetKey(pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS) || (glfwGetKey(pWindow, GLFW_KEY_S ) == GLFW_PRESS)){
        direction -= 3.0f;
    }
    return direction;
}

void Application::getJump() {
    if(!pTank->getIsInAir())
    {
        if (glfwGetKey(pWindow, GLFW_KEY_SPACE ) == GLFW_PRESS)
        {
            pTank->setIsInAir(true);
            this->downForce = pTank->getJumpPower();
        }
    }

}

/*
float Application::getJump() {
    float jumpFactor = 2.0f;

    if(isJumpingOld) {
        isJumpingOld = false;
        return -jumpFactor;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE ) == GLFW_PRESS){
        isJumpingOld = true;
        return jumpFactor;
    }
    isJumpingOld = false;
    
    return 0.0f;
}

bool Application::isJumping()
{
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE ) == GLFW_PRESS){
        isJumpingOld = true;
        return true;
    }
    isJumpingOld = false;
    return false;
}
*/
bool Application::collisionDetection(Tank* model1, Model* model2)
{
    Vector vec1 = model1->transform().translation();
    Vector vec2 = model2->transform().translation();
    
    Vector size1 = model1->boundingBox().size();
    Vector size2 = model2->boundingBox().size();
    
    // Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
    return vec1.X - size1.X/2 < vec2.X + size2.X/2 &&
    vec2.X - size2.X/2 < vec1.X + size1.X/2 &&
    vec1.Z - size1.Z/2 < vec2.Z + size2.Z/2 &&
    vec2.Z - size2.Z/2 < vec1.Z + size1.Z/2;
    
/*
     for(ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
     {
            if((*it) != model)
            {
                const AABB& m2 = (*it)->boundingBox();
            
                collision = m1.Min.X < m2.Max.X &&
                m2.Min.X < m1.Max.X &&
                m1.Min.Y < m2.Max.Y &&
                m2.Min.Y < m1.Max.Y &&
                m1.Min.Z < m2.Max.Z &&
                m2.Min.Z < m1.Max.Z;
     
                std::cout << " Collision is " << collision << " " <<std::endl;
            }
     
            //Abbruch, wenn Kollision entdeckt wurde
            if(collision) {
                return true;
            }
    }
    return collision;
*/
}
