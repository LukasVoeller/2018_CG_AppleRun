//
//  Scene.h
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <map>
#include <list>
#include <string.h>
#include <assert.h>

#include "BaseModel.h"

#include "SceneNode.h"
#include "Coin.h"
#include "MovingItem.h"

#include "OutlineShader.h"

class Scene : public BaseModel
{
public:
	typedef std::list<SceneNode*> NodeList;
	typedef std::list<Coin*> CoinList;
	typedef std::list<MovingItem*> MovingItemList;
	Scene();
	virtual ~Scene();
	bool addSceneFile(const char* Scenefile);
	virtual void draw(const BaseCamera& Cam); //komplette Szene
	CoinList getCoins() {return mCoins;}
	NodeList getObstacles() {return mBarriers; }
	NodeList getDeathItems() {return mDeathItems; }
	MovingItemList getMovingItems() {return mMovingItems; }
protected:
	void draw(SceneNode* pNode); // brauch ich das?//einzelner Knoten
	SceneNode m_Root;
	std::map<std::string, Model*> m_Models;
	
	CoinList mCoins;
	NodeList mBarriers;
	NodeList mDeathItems;
	MovingItemList mMovingItems;
private:
	SceneNode* findNode(char* parentID, SceneNode *node); //helper
	SceneNode* deleteNodes(SceneNode* parent);
};

#endif /* Scene_h */
