//
//  Scene.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include <stdio.h>

#include "Scene.h"
#include <string.h>
#include "Model.h"
#include <assert.h>
#include <stdio.h>

Scene::Scene()
{
	m_Models = std::map<std::string, Model*>();
	m_Root = SceneNode();
	m_Root.setName("Root");
}

Scene::~Scene()
{
	// Models löschen (it->first, it->second) = (key,value)
	for(std::map<std::string, Model*>::iterator it = this->m_Models.begin(); it != this->m_Models.end(); ++it)
	{
		delete it->second;
	}
	
	//Scene nodes löschen
	deleteNodes(&m_Root);
}

bool Scene::addSceneFile(const char* Scenefile)
{
	char Line[512];
	
	FILE* sceneFile = fopen(Scenefile, "r");
	while (fgets(Line, sizeof(Line), sceneFile)) {
		if (strstr(Line, "NODE")) // //NODE nur in "Node"-Lines
		{
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
			SceneNode* sceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, parent, m_Models[ModelID]);
		}
		if(strstr(Line, "FILE")) //FILE nur in "Model"-Lines
		{
			char Modelfile[256];
			char ModelID[256];
			sscanf(Line, "MODEL ID=%s FILE=%s", ModelID, Modelfile);
			
			Model* m = new Model(Modelfile);
			m->shader(this->shader());
			
			m_Models.insert(std::pair<std::string, Model*>(ModelID, m));
		}
	}
	fclose(sceneFile);
	return true;
}

void Scene::draw(const BaseCamera& Cam)
{
	m_Root.draw(Cam);
}

/*
 Iterative Suchfunktion für Parentnodes
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
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it)
	{
		SceneNode* tmp = findNode(parentID, *it);
		if (tmp != NULL) {
			return tmp;
		}
	}
	return NULL;
}

SceneNode* Scene::deleteNodes(SceneNode* node)
{
	std::set<SceneNode*>::iterator it;
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it)
	{
		SceneNode* tmp = deleteNodes(*it);
		if (tmp != NULL) {
			delete tmp;
		}
	}
	return NULL;
}
