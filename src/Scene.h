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
#include "BaseModel.h"
#include "Model.h"

#include "SceneNode.h"

class Scene : public BaseModel
{
public:
	Scene();
	virtual ~Scene();
	bool addSceneFile(const char* Scenefile);
	virtual void draw(const BaseCamera& Cam); //komplette Szene
protected:
	void draw(SceneNode* pNode); //einzelner Knoten
	SceneNode m_Root;
	std::map<std::string, Model*> m_Models;
private:
	SceneNode* findNode(char* parentID, SceneNode *node); //helper
	SceneNode* deleteNodes(SceneNode* parent);
};

#endif /* Scene_h */
