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
#include "SceneNode.h"
#include "Coin.h"

class CollisionHandler {
public:
	CollisionHandler();
	~CollisionHandler();
	
	bool collisionDetection(Character* c, SceneNode* n, float delta);
	void handleCollisionWithBarrier(Character* c, SceneNode* n);
	
	void handleCollisionWithCoin(Coin* c2);
	void handleCoinMoving(Coin* c);
};

#endif /* Collision_h */
