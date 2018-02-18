//
//  SceneNode.h
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef SceneNode_h
#define SceneNode_h

#include <iostream>
#include "Matrix.h"
#include <set>
#include <vector>
#include <map>
#include <string>
#include "BaseModel.h"
#include "Model.h"

class SceneNode
{
public:
	SceneNode();
	SceneNode( const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel, std::string modelType="MODEL");
	
	//getter
	const Matrix& getLocalTransform() const;
	Matrix getGlobalTransform(bool scaling = true) const;
	const SceneNode* getParent() const;
	Model* getModel() const;
	const std::string& getName() const;
	const Vector& getScaling() const;
	const std::string& getModelType() const; //löschen
	const Vector& getLatestPosition() const; //für Coins
	const bool isCollected() const; //für Coins
	
	//setter
	void setLocalTransform( const Vector& Translation, const Vector& RotationAxis, const float RotationAngle );
	void setLocalTransform( const Matrix& LocalTransform);
	void setParent( SceneNode* pNode);
	const std::set<SceneNode*>& getChildren() const;
	void addChild(SceneNode* pChild);
	void removeChild(SceneNode* pChild);
	void setModel( Model* pModel);
	void setName( const std::string& Name);
	void setScaling( const Vector& Scaling);
	void setModelType(const std::string& modelType); //löschen
	void setCollected (const bool collected); //für Coins
	void setLatestPosition(const Vector& pos); //für Coins
	void draw(const BaseCamera& Cam);
protected:
	std::string m_Name;
	std::string modelType;
	Model* m_pModel;
	SceneNode* m_pParent;
	std::set<SceneNode*> m_Children;
	Matrix m_LocalTransform;
	Vector m_Scaling;
	
	bool collected = false;
	Vector latestPosition;
};

#endif /* SceneNode_h */
