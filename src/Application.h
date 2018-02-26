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
#include "EgoCam.h"
#include "GUIEvents.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Game.h"
#include "Control.h"
#include "Character.h"

class Application
{
public:
	Application(GLFWwindow* pWin);
	typedef std::list<BaseModel*> ModelList;
	typedef std::list<SceneNode*> NodeList;
	typedef std::list<Coin*> CoinList;
	typedef std::list<MovingItem*> MovingItemList;
	void getInputPitchRollForward(float& pitch, float& roll, float& forward);
	void start();
	void update(float dtime);
	void draw();
	void end();
	
	void plattformsHover();
	
protected:
	EgoCam egocam;
	GUIEvents gui;
	Scene* pScene;
	ModelList models;
	GLFWwindow* pWindow;
	BaseModel* lineGrid;
	BaseModel* pModel;
	ShadowMapGenerator shadowGenerator;
	
	void createScene();
	void createNormalTestScene();
	void createShadowTestScene();
	void reset(float dtime);
	bool collisionDetection(Tank* model, Model* model2); //ggf. löschen
	bool collisionDetection(Tank* model, SceneNode* model2);
	void calcLegalPosition(Tank* model, SceneNode* model2); //ggf. löschen
	void collisionHandling(Tank* model, SceneNode* model2);
	Matrix calcCharacterViewMatrix(Tank* character);
	double calcDeltaTime();
	Vector calc3DRay( float x, float y, Vector& Pos);
	
	// Game Variables
	float gravity = GRAVITY;
	float downForce = DOWNFORCE;
	float terrainHeight = TERRAIN_HEIGHT;
	
	// Time
	double oldTime = 0;
	float time;
	int coolDownTimer = 0;
	
	// Tank
	Character* pTank;
	
	// Obstacle List
	NodeList pBarriers;
	
	// Coins
	CoinList pCoins;
	unsigned int allCoins;
	unsigned int collectedCoins;
	
	//DeathBlock
	NodeList pDeathblocks;
	
	MovingItemList pMovingItems;
	
	// Testmodel
	Model* pTest;
	
	//GameLogik
	Game game;
	
	//Steuerung des Characters
	Control playerControl;
	
private:
	float leftRight;
	float forwardBackward;
	float deltaTime;
	
	Matrix characterViewMatrix;

};

#endif /* Application_hpp */
