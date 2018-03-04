//
//  Collision.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 02.03.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include "CollisionHandler.h"

CollisionHandler::CollisionHandler() {
	
}

CollisionHandler::~CollisionHandler() {
	
}

/****** Kollision mit Scenenode *********/
/***** DELTA für Sicherheitsabstand **********/
/*** DELTA ist Parameter, damit für bewegende Gegenstände eine höheres Delta verwendet werden kann **/
//IDEE: nehme nicht Position und größe, sondern Position und Boundingbox.min bzw. max...
bool CollisionHandler::collisionDetection(Character* model1, SceneNode* node, float delta) {
	Vector vec1 = model1->transform().translation();
	Vector vec2 = node->getLocalTransform().translation();
	
	AABB bb1 = model1->getScaledBoundingBox();
	AABB bb2 = node->getScaledBoundingBox();
	
	//Ähnlich von hier https://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung
	return (vec1.X + bb1.Min.X < vec2.X + bb2.Max.X + delta &&
			vec2.X + bb2.Min.X < vec1.X + bb1.Max.X + delta &&
			vec1.Y + bb1.Min.Y < vec2.Y + bb2.Max.Y + delta &&
			vec2.Y + bb2.Min.Y < vec1.Y + bb1.Max.Y + delta &&
			vec1.Z + bb1.Min.Z < vec2.Z + bb2.Max.Z + delta &&
			vec2.Z + bb2.Min.Z < vec1.Z + bb1.Max.Z + delta);

}

void CollisionHandler::handleCollisionWithBarrier(Character* model1, SceneNode* model2) {
//	Matrix t;
//	Matrix m = model1->transform();
//	
//	Vector pos1 = model1->getLatestPosition();
//	Vector pos2 = model2->getLocalTransform().translation();
//	Vector size2 = model2->getScaledBoundingBox().size();
//	float bMaxY = pos2.Y + 0.5f* model2->getScaledBoundingBox().size().Y;
//	float bMinY = pos2.Y - 0.5f* model2->getScaledBoundingBox().size().Y;
//	float cMinY = pos1.Y - 0.5f* model1->getScaledBoundingBox().size().Y;
//	float cMaxY = pos1.Y + 0.5f* model1->getScaledBoundingBox().size().Y;
//	
//	if(pos1.Y <= TERRAIN_HEIGHT && !model1->getIsInAir()) {
//		std::cout << "seite" << std::endl;
//		t.translation(-6*forwardBackward*deltaTime, 0, -6*leftRight*deltaTime);
//		model1->transform(m*t);
//		return;
//	}
//	
//	// Von oben bzw. in der oberen Hälfte
//	else if(bMaxY > cMinY && cMinY > pos2.Y ) {
//		std::cout << "oben" << std::endl;
//		model1->getControl().setJumpPower(0.0f);
//		model1->setIsInAir(false);
//		return;
//	}
//	
//	// Von unten -> begrenze die Sprunghöhe
//	else if(bMinY < cMaxY  && cMaxY < pos2.Y) {
//		std::cout << "von unten" << std::endl;
//		model1->getControl().setJumpPower(-3.0f);
//	}
//	
//	// Auf der Erde
//	else {
//		std::cout << "seite..." << std::endl;
//		//t.translation(-6*forwardBackward*deltaTime, 0, -6*leftRight*deltaTime);
//		model1->transform(m*t);
//	}
}

void CollisionHandler::handleCollisionWithCoin(Coin* coin) {
	Matrix trans;
	coin->setCollected(true);
	
	trans.translation(0, 2.5f, 0);
	coin->setLocalTransform(coin->getLocalTransform()*trans);
}

void CollisionHandler::handleCoinMoving(Coin* c) {
	Matrix coinMat = c->getLocalTransform();
	Matrix t;
	float newHeight = coinMat.translation().Y - 0.25f;
	
	t.translation(coinMat.translation().X, newHeight, coinMat.translation().Z);
	c->setLocalTransform(t);
	c->setLatestPosition(coinMat.translation());
}

bool CollisionHandler::collisionWithBorder(Character* c) {
	Vector pos = c->transform().translation();
	bool nearBorder = false;
	float x = 0; float z = 0;
	if(pos.X <= BORDER_MIN_X + DELTA) {x = 1.0f; nearBorder = true;};
	if(pos.X >= BORDER_MAX_X - DELTA) {x = -1.0f; nearBorder = true;};
	if(pos.X <= BORDER_MIN_Z + DELTA) {z = 1.0f; nearBorder = true;};
	if(pos.X >= BORDER_MAX_Z - DELTA) {z = -1.0f; nearBorder = true;};
	if(nearBorder) {
		Matrix t;
		t.translation(x, 0.0f, z);
		c->transform(c->transform()*t);
	}
	return nearBorder;
}
