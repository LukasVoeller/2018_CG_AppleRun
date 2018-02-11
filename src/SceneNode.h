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
#include "BaseModel.h"
#include "Model.h"

class SceneNode
{
public:
	SceneNode();
	SceneNode( const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel);
	
	//getter
	const Matrix& getLocalTransform() const;
	Matrix getGlobalTransform() const;
	Matrix getGlobalTransformWithoutScaling() const;
	const SceneNode* getParent() const;
	const Model* getModel() const;
	const std::string& getName() const;
	const Vector& getScaling() const;
	
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
	void draw(const BaseCamera& Cam);
protected:
	std::string m_Name;
	Model* m_pModel;
	SceneNode* m_pParent;
	std::set<SceneNode*> m_Children;
	Matrix m_LocalTransform;
	Vector m_Scaling;
};

#endif /* SceneNode_h */
