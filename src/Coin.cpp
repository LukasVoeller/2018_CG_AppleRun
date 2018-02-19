//
//  Coin.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 19.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include <stdio.h>
#include "Coin.h"

Coin::Coin():SceneNode()
{
	setLatestPosition(this->getLocalTransform().translation());
}

Coin::Coin(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel):SceneNode(Name, Translation, RotationAxis, RotationAngle, Scale, pParent, pModel)
{
	setLatestPosition(this->getLocalTransform().translation());
}

Coin::~Coin() {

}

const Vector& Coin::getLatestPosition() const
{
	return latestPosition;
}

void Coin::setLatestPosition(const Vector& pos)
{
	latestPosition = pos;
}

const bool Coin::isCollected() const
{
	return collected;
}
void Coin::setCollected(const bool isCollected)
{
	collected = isCollected;
}
