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
#include "BaseModel.h"
#include "Model.h"

#include "SceneNode.h"

class Scene : public BaseModel
{
public:
	typedef std::list<SceneNode*> NodeList;
	Scene();
	virtual ~Scene();
	bool addSceneFile(const char* Scenefile);
	virtual void draw(const BaseCamera& Cam); //komplette Szene
	NodeList getCoins() {return mCoins;}
	NodeList getObstacles() {return mBarriers; }
	NodeList getDeathItems() {return mDeathItems; }
protected:
	//void draw(SceneNode* pNode); // brauch ich das?//einzelner Knoten
	SceneNode m_Root;
	std::map<std::string, Model*> m_Models;
	
	NodeList mCoins;
	NodeList mBarriers;
	NodeList mDeathItems;
private:
	SceneNode* findNode(char* parentID, SceneNode *node); //helper
	SceneNode* deleteNodes(SceneNode* parent);
};

#endif /* Scene_h */
