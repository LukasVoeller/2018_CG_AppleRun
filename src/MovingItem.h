//
//  MovingItem.h
//  CGXcode
//
//  Created by Sandra Tieben on 19.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef MovingItem_h
#define MovingItem_h

#include "SceneNode.h"

class MovingItem : public SceneNode
{
public:
	MovingItem();
	MovingItem(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel);
	~MovingItem();
	
	/* Getter */
	const Vector& getLatestPosition() const;
	const bool isMovingUp() const;
	
	/* Setter */
	void setMoveUp(const bool up); 	// Paletten
	void setLatestPosition(const Vector& pos);
private:
	Vector latestPosition; 			// Brauch ich das?
	bool moveUp = true;
};

#endif /* MovingItem_h */
