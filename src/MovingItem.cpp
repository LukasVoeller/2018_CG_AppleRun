//
//  MovingItem.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 19.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include <stdio.h>
#include "MovingItem.h"

MovingItem::MovingItem():SceneNode()
{
	setLatestPosition(this->getLocalTransform().translation());
}

MovingItem::MovingItem(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel):SceneNode(Name, Translation, RotationAxis, RotationAngle, Scale, pParent, pModel)
{
	setLatestPosition(this->getLocalTransform().translation());
	
}

MovingItem::~MovingItem() {
	
}

void MovingItem::movingUp() {
	Vector t = this->getLocalTransform().translation();
	this->setMoveUp(true);
	
	Matrix trans;
	trans.translation(t.X, t.Y + 0.3f, t.Z);
	this->setLocalTransform(trans);
}

void MovingItem::moving() {
	Vector t = this->getLocalTransform().translation();
	float heigth = t.Y;
	
	if (heigth > 13.0f) {
		this->setMoveUp(false);
	}
	if(heigth < 1.5f) {
		this->setMoveUp(true);
	}
	
	heigth = this->isMovingUp() ? heigth + 0.05 : heigth - 0.05;
	
	Matrix trans;
	trans.translation(t.X, heigth, t.Z);
	this->setLocalTransform(trans);
	setLatestPosition(this->getLocalTransform().translation());
}

const Vector& MovingItem::getLatestPosition() const
{
	return latestPosition;
}

void MovingItem::setLatestPosition(const Vector& pos)
{
	latestPosition = pos;
}

const bool MovingItem::isMovingUp() const
{
	return moveUp;
}
void MovingItem::setMoveUp(const bool up)
{
	moveUp = up;
}

void MovingItem::update(float dt) {
	this->moving();
}


