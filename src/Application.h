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
#include "Constants.h"
#include "PhongShader.h"
#include "OutlineShader.h"
#include "ConstantShader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BaseModel.h"
#include "ShadowMapGenerator.h"
#include "Tank.h"
#include "Coin.h"
#include "DeathBlock.h"
#include "EgoCam.h"
#include "GUIEvents.h"
#include "Scene.h"

class Application{
public:
	Application(GLFWwindow* pWin);
	typedef std::list<BaseModel*> ModelList;
	void getInputPitchRollForward(float& pitch, float& roll, float& forward);
	void start();
	void update(float dtime);
	void draw();
	void end();
	
protected:
	EgoCam egocam;
	GUIEvents gui;
	ModelList models;
	GLFWwindow* pWindow;
	BaseModel* lineGrid;
	BaseModel* pModel;
	ShadowMapGenerator shadowGenerator;
	
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
	void reset(float dtime);
	bool collisionDetection(Tank* model, Model* model2);
	double calcDeltaTime();
	Vector calc3DRay( float x, float y, Vector& Pos);
	
	// Game Variables
	float gravity = GRAVITY;
	float downForce = DOWNFORCE;
	float terrainHeight = TERRAIN_HEIGHT;
	
	// Time
	double oldTime = 0;
	float time;
	int actionTimer;
	
	// Tank
	Tank* pTank;
	float getForwardBackward();
	float getLeftRight();
	void getJump();
	
	// Testmodels
	Model* pBarrier1;
	Model* pBarrier2;
	
	// Obstacle List
	ModelList pBarriers;
	
	// Coins
	Coin* coin;
	ModelList pCoins;
	unsigned int allCoins;
	unsigned int collectedCoins;
	
	//DeathBlock
	DeathBlock* deathblock;
	ModelList pDeathblocks;
	
	// Testmodel
	Model* pTest;
};

#endif /* Application_hpp */
