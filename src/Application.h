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
#include "Camera.h"
#include "PhongShader.h"
#include "ConstantShader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BaseModel.h"
#include "ShadowMapGenerator.h"
#include "Tank.h"

#include "EgoCam.h"

class Application{
public:
	typedef std::list<BaseModel*> ModelList;
	Application(GLFWwindow* pWin);
	void start();
	void update(float dtime);
	void draw();
	void end();
	
	void getInputPitchRollForward(float& pitch, float& roll, float& forward);
	
protected:
	Camera Cam;
	EgoCam Egocam;
	ModelList Models;
	GLFWwindow* pWindow;
	BaseModel* LineGrid;
	BaseModel* pModel;
	ShadowMapGenerator ShadowGenerator;
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
	Vector calc3DRay( float x, float y, Vector& Pos);
	bool collisionDetection(Tank* model, Model* model2);
	double calcDeltaTime();
	double oldTime = 0;
	float time;
	
	// Tank
	Tank* pTank;
	float getForwardBackward();
	float getLeftRight();
	void getJump();
	
	// Testmodels
	Model* pBarrier1;
	Model* pBarrier2; /* only for testing */
	
	// Obstacle List
	ModelList pBarriers;
	
	// Coins
	Model* coin;
	ModelList pCoins;
	
	// Game Variables
	float gravity = -15;
	float downForce = 0.0f;
	float terrainHeight = 0.6f;
};

#endif /* Application_hpp */
