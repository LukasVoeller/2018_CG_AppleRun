//
//  SceneNode.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include <stdio.h>
#include "SceneNode.h"

SceneNode::SceneNode() : m_Name(""), m_Scaling(1, 1, 1), m_pParent(NULL), m_pModel(NULL)
{
	//TODO: Verstehen :D
//	setScaling(Vector(2, 2, 2));
	setLocalTransform(Vector(0, 0, 0), Vector(0, 1, 0), 0);
}

SceneNode::SceneNode(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel)
{
	setName(Name);
	setLocalTransform(Translation, RotationAxis, RotationAngle);
	setScaling(Scale);
	setParent(pParent);
	setModel(pModel);
}

const Matrix& SceneNode::getLocalTransform() const
{
	return m_LocalTransform;
}

void SceneNode::setLocalTransform(const Vector& Translation, const Vector& RotationAxis, const float RotationAngle)
{
	Matrix rotation, translation;
	rotation.rotationAxis(RotationAxis, RotationAngle);
	translation.translation(Translation);
	this->setLocalTransform(translation * rotation);
}

void SceneNode::setLocalTransform(const Matrix& LocalTransform)
{
	m_LocalTransform = LocalTransform;
}

Matrix SceneNode::getGlobalTransform() const
{
	Matrix scaling, localTransform;
	scaling = scaling.scale(this->getScaling());
	localTransform = this->getLocalTransform();
	
	if (this->getParent() == NULL) {
		return localTransform * scaling;
	}
	
	//Matrix parentGlobalTransform = this->getParent()->getGlobalTransform();
	Matrix parentGlobalTransform = this->getParent()->getGlobalTransformWithoutScaling();
	return parentGlobalTransform * localTransform * scaling;
}

Matrix SceneNode::getGlobalTransformWithoutScaling() const
{
	Matrix localTransform;
	localTransform = this->getLocalTransform();

	if (this->getParent() == NULL) {
		return localTransform;
	}

	Matrix parentGlobalTransform = this->getParent()->getGlobalTransformWithoutScaling();
	return parentGlobalTransform * localTransform;
}

const SceneNode* SceneNode::getParent() const
{
	return m_pParent;
}

void SceneNode::setParent(SceneNode* pNode)
{
	m_pParent = pNode;
	pNode->addChild(this);
}

const std::set<SceneNode*>& SceneNode::getChildren() const
{
	return m_Children;
}

void SceneNode::addChild(SceneNode* pChild)
{
	m_Children.insert(pChild);
}

void SceneNode::removeChild(SceneNode* pChild)
{
	m_Children.erase(pChild);
}

void SceneNode::setModel(Model* pModel)
{
	m_pModel = pModel;
}

const Model* SceneNode::getModel() const
{
	return m_pModel;
}

void SceneNode::setName(const std::string& Name)
{
	m_Name = Name;
}

const std::string& SceneNode::getName() const
{
	return m_Name;
}

const Vector& SceneNode::getScaling() const
{
	return m_Scaling;
}
void SceneNode::setScaling(const Vector& Scaling)
{
	m_Scaling = Scaling;
}

void SceneNode::draw(const BaseCamera& Cam)
{
	if (m_pModel != NULL) {
		m_pModel->transform(this->getGlobalTransform());
		m_pModel->draw(Cam);
	}
	
	//Kinder iterativ zeichnen
	std::set<SceneNode *>::iterator it;
	for (it = this->getChildren().begin(); it != this->getChildren().end(); ++it)
	{
		(*it)->draw(Cam);
	}
}
