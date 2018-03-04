//
//  Scene.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include "Scene.h"

Scene::Scene() {
	m_Models = std::map<std::string, Model*>();
	m_Root = SceneNode();
	m_Root.setName("Root");
}

Scene::~Scene() {
	// Models löschen (it->first, it->second) = (key,value)
	for(std::map<std::string, Model*>::iterator it = this->m_Models.begin(); it != this->m_Models.end(); ++it) {
		delete it->second;
	}
	
	//Scene nodes löschen
	deleteNodes(&m_Root);
	delete sl;
}

bool Scene::addSceneFile(const char* Scenefile) {
	char Line[512];
	
	FILE* sceneFile = fopen(Scenefile, "r");
	while (fgets(Line, sizeof(Line), sceneFile)) {
		if (strstr(Line, "NODE")) { 	// NODE nur in "Node"-Lines
			Vector Pos, Scale, RotAxis;
			float Angle;
			char NodeID[256];
			char ParentID[256];
			char ModelID[256];
			sscanf(Line, "NODE ID=%s PARENTID=%s MODELID=%s TRANSLATION=%f %f %f ROTATIONAXIS=%f %f %f ROTATIONANGLE=%f SCALE=%f %f %f",
				   NodeID, ParentID, ModelID,
				   &Pos.X, &Pos.Y, &Pos.Z,
				   &RotAxis.X, &RotAxis.Y, &RotAxis.Z,
				   &Angle,
				   &Scale.X, &Scale.Y, &Scale.Z);
			
			SceneNode* parent = findNode(ParentID, &m_Root);
			if (parent == NULL) {
				parent = &m_Root;
			}
			
			//Jeden SceneNode auf einer eigenen Liste speichern (Coin, Barrier, DeathItem, ...)
			if(strstr(ModelID, "apple")) {
				Coin* sceneNode = new Coin(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
				mCoins.push_back(sceneNode);
			}
			else if(strstr(ModelID, "barrel")) {
				SceneNode* sceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
				mDeathItems.push_back(sceneNode);
			}
			else if(strstr(ModelID, "collisioncube") || strstr(ModelID, "factory") || strstr(ModelID, "woodcube") ) {
				SceneNode* sceneNode = new SceneNode(ModelID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
				mBarriers.push_back(sceneNode);
			}
			else if(strstr(ModelID, "pallet")) {
				MovingItem* sceneNode = new MovingItem(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
				mMovingItems.push_back(sceneNode);
			}
			else if(strstr(ModelID, "streetlamp")) {
				SceneNode* sceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);

				sl = new SpotLight();
				sl->position(Vector(Pos.X - 5.0f, 5.0f, Pos.Z));
				sl->color(Color(1.0, 0.7, 1.0f));
				sl->direction(Vector(-1, -4, 0));
				sl->innerRadius(30.0f);
				sl->outerRadius(45.0f);
				ShaderLightMapper::instance().addLight(sl);
			}
			else {
				SceneNode* sceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
			}

		}
		
		if(strstr(Line, "FILE")) { //FILE nur in "Model"-Lines
			char Modelfile[256];
			char ModelID[256];
			char ModelType[256];
			
			memset(ModelID, '\0', sizeof(ModelID));
			memset(Modelfile, '\0', sizeof(Modelfile));
			memset(ModelType, '\0', sizeof(ModelType));
			
			sscanf(Line, "MODEL ID=%s FILE=%s", ModelID, Modelfile);
			
			Model* m;
			m = new Model(Modelfile);
			m->shader(this->shader());
			if(strstr(ModelID, "apple")) {
				m->shader(new OutlineShader());
			}
			
			m_Models.insert(std::pair<std::string, Model*>(ModelID, m));
		}
	}
	fclose(sceneFile);
	return true;
}

void Scene::draw(const BaseCamera& Cam) {
	m_Root.draw(Cam);
}

void Scene::draw(SceneNode* pNode) {
//	Matrix globalTrans = pNode->getGlobalTransform();
//	pNode->getModel()->transform(globalTrans);
//
//	std::set<SceneNode*>::iterator it;
//	for (it = pNode->getChildren().begin(); it != pNode->getChildren().end(); ++it)
//	{
//		draw(*it);
//	}
}

/*
 Rekursive Suchfunktion für Parentnodes
 Wenn ParentID = NULL -> root ist Parent
 */
SceneNode* Scene::findNode(char* parentID, SceneNode* node) {
	if (strcmp(parentID, "NULL") == 0) {
		return &m_Root;
	}
	
	if (strcmp(parentID, node->getName().c_str()) == 0) {
		return node;
	}
	
	std::set<SceneNode*>::iterator it;
	
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		SceneNode* tmp = findNode(parentID, *it);
		if (tmp != NULL) {
			return tmp;
		}
	}
	return NULL;
}

SceneNode* Scene::deleteNodes(SceneNode* node) {
	std::set<SceneNode*>::iterator it;
	
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		SceneNode* tmp = deleteNodes(*it);
		if (tmp != NULL) {
			delete tmp;
		}
	}
	return NULL;
}

