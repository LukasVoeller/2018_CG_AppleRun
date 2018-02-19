//
//  Coin.h
//  CGXcode
//
//  Created by Sandra Tieben on 19.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Coin_h
#define Coin_h

#include "SceneNode.h"

class Coin : public SceneNode
{
public:
	Coin();
	Coin(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel);
	~Coin();	
	
	/* Getter */
	const Vector& getLatestPosition() const;
	const bool isCollected() const;
	
	/* Setter */
	void setCollected (const bool collected);
	void setLatestPosition(const Vector& pos);
private:
	bool collected = false;
	Vector latestPosition;
};

#endif /* Coin_h */

