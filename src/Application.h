//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "ShadowMapGenerator.h"
#include "tank.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
protected:
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;
    
    //Panzer mit Bewegung
    Tank* pTank;
    double calcDeltaTime();
    double oldTime = 0;
    float getForwardBackward();
    float getLeftRight();
    float getJump();
    bool isJumping(); //TODO löschen
    bool isJumpingOld = false;
    //Für Schussrohr
    Vector calc3DRay( float x, float y, Vector& Pos);
    
    //Kollisionserkennung
    bool collisionDetection(Tank* model, Model* model2);
    
    //Testobjekt für Kollision
    Model* pBarrier;
};

#endif /* Application_hpp */
