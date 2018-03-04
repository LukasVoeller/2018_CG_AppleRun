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
#include "Character.h"
#include "EgoCam.h"
#include "GUIEvents.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Game.h"
#include "Control.h"
#include "Character.h"

class Application {
public:
	Application(GLFWwindow* pWin);
	typedef std::list<BaseModel*> ModelList;
	typedef std::list<SceneNode*> NodeList;
	typedef std::list<Coin*> CoinList;
	typedef std::list<MovingItem*> MovingItemList;
	void start();
	void update(float dtime);
	void draw();
	void end();
	
	void plattformsHover();
	void updateCharacter(Character* c, Matrix m, float jumppower, bool hovering);
	
	float getDeltaTime() {return deltaTime;};

	
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
	void createShadowTestScene();
//	//void calcLegalPosition(Character* model, SceneNode* model2); //ggf. löschen
//	//void collisionHandling(Character* model, SceneNode* model2, float delta);
//	void palletCollisionHandling(Character* model, MovingItem* model2);
	Matrix calcCharacterViewMatrix(Character* character);
	double calcDeltaTime();
	Vector calc3DRay( float x, float y, Vector& Pos);
	
	// Time
	double oldTime = 0;
	float time;
	int coolDownTimer = 0;
	
	// Character
	Character* pCharacter;
	
	// Obstacle List
	NodeList pBarriers;
	
	// Different objects
	CoinList pCoins;
	NodeList pDeathblocks;
	MovingItemList pMovingItems;

	//GameLogic
	Game game;

	
private:
	float deltaTime;
	
	void collisionWithBarrier();
	void collisionWithBarrel();
	void collisionWithCoin();
	void collisionWithPallet();

};

#endif /* Application_hpp */
