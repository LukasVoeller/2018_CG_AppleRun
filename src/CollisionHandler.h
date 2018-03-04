//
//  Collision.h
//  CGXcode
//
//  Created by Sandra Tieben on 02.03.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Collision_h
#define Collision_h

#include "Character.h"
#include "Coin.h"
#include "Control.h"
#include "MovingItem.h"
#include "SceneNode.h"

class CollisionHandler {
public:
	CollisionHandler();
	~CollisionHandler();
	
	bool collisionDetection(Character* c, SceneNode* n, float delta);
	void handleCollisionWithBarrier(Character* c, SceneNode* n, float delta, Control* control);
	void handleCollisionWithPalette(Character* c, MovingItem* n, float delta, Control* control);
	
	void handleCollisionWithCoin(Coin* c2);
	void handleCoinMoving(Coin* c);
	
	bool collisionWithBorder(Character* c);


};

#endif /* Collision_h */
