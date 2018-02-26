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


