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

/* Behandlung aller Fälle
 * a. von Oben auf eine Kiste springen
 * b. seitlich dagegen springen (also in der Luft)
 * c. seitlich dagegen fahren (auf dem Boden
 * d. von unten nach oben dagegen springen */
void CollisionHandler::handleCollisionWithBarrier(Character* model1, SceneNode* model2, float delta, Control* playerControl) {
	Matrix t;
	Matrix m = model1->transform();
	
	Vector pos1 = model1->getLatestPosition();
	Vector pos2 = model2->getLocalTransform().translation();
	
	AABB bb1 = model1->getScaledBoundingBox();
	AABB bb2 = model2->getScaledBoundingBox();
	
	float x = m.translation().X;
	float z = m.translation().Z;
	float d = 0.8f;
	

		// Von oben bzw. in der oberen Hälfte
		if(pos2.Y + bb2.Max.Y + delta > pos1.Y + bb1.Min.Y && pos1.Y + bb1.Min.Y + delta > pos2.Y + bb2.Min.Y ) {
			//if(pos1.Y + delta > pos2.Y + bb2.Max.Y) {
			std::cout << "oben" << std::endl;
			if(model1->getUnderground() != model2) {
				model1->setJump(0.0f);
				playerControl->setJumpPower(0.0f);
				model1->setUnderground(model2);
			}
			model1->setIsInAir(false);
			
		}
		
		// Von unten -> begrenze die Sprunghöhe
		//else if(model1->getIsInAir() && pos2.Y + bb2.Min.Y < pos1.Y + bb1.Max.Y + delta && pos1.Y + bb1.Max.Y < pos2.Y + delta) {
		else if(model1->getIsInAir() && pos2.Y + bb2.Min.Y + delta > pos1.Y) {
			std::cout << "von unten" << std::endl;
			playerControl->setJumpPower(-10.0f);
		}
		
		// Auf der Erde
		else {
			if(pos1.X < pos2.X + bb2.Min.X) {
				x = pos2.X + bb2.Min.X - bb2.Max.X - d;
			}
			if(pos1.X > pos2.X + bb2.Max.X) {
				x = pos2.X + bb2.Max.X - bb2.Min.X + d;
			}
			if(pos1.Z < pos2.Z + bb2.Min.Z) {
				z = pos2.Z + bb2.Min.Z - bb2.Max.Z - d;
			}
			if(pos1.Z > pos2.Z + bb2.Max.Z) {
				z = pos2.Z + bb2.Max.Z - bb2.Min.Z + d;
			}
			
			float angle = (m.m00 > 0.2 || m.m00 < -0.2f) ? acos(m.m00) : asin(m.m02);
			
			playerControl->setLeftRight(0.0f);
			playerControl->setForwardBackward(0.0f);
			t.translation(x, 0.0f, z);
			Matrix r;
			r.rotationY(angle);
			model1->transform(t*r);
			
			std::cout << "seite..." << angle << " "<< asin(m.m02) << std::endl;
		}


}

void CollisionHandler::handleCollisionWithPalette(Character* model1, MovingItem* model2, float delta, Control* control) {
	Matrix t;
	Matrix m = model1->transform();
	
	Vector pos1 = model1->getLatestPosition();
	Vector pos2 = model2->getLocalTransform().translation();
	
	AABB bb1 = model1->getScaledBoundingBox();
	AABB bb2 = model2->getScaledBoundingBox();
	
	float x = m.translation().X;
	float z = m.translation().Z;
	float d = 0.8f;
	
	float angle = (m.m00 > 0.2 || m.m00 < -0.2f) ? acos(m.m00) : asin(m.m02);
	Matrix r;
	r.rotationY(angle);
	
	if(model1->getPallet() == NULL) {
		if(pos1.Y + DELTA > pos2.Y + bb2.Max.Y) {
			std::cout << "oben" << std::endl;
//			if(model1->getPallet() != model2) {
//				model1->setJump(0.0f);
//				control->setJumpPower(0.0f);
//				model1->setPallet(model2);
//			}
			control->setJumpPower(0.0f);
			model1->setIsInAir(false);
			model1->setHovering(true);
			model1->setPallet(model2);
		}
		// Von unten -> begrenze die Sprunghöhe
		else if(pos2.Y + bb2.Min.Y + DELTA > pos1.Y) { //model1->getIsInAir()){ //  &&
			std::cout << "von unten" << std::endl;
			model2->movingUp();
			control->setJumpPower(-10.0f);
		}
		// Auf der Erde
		else {
			if(pos1.X < pos2.X + bb2.Min.X) {
				x = pos2.X + bb2.Min.X - bb2.Max.X - d;
			}
			if(pos1.X > pos2.X + bb2.Max.X) {
				x = pos2.X + bb2.Max.X - bb2.Min.X + d;
			}
			if(pos1.Z < pos2.Z + bb2.Min.Z) {
				z = pos2.Z + bb2.Min.Z - bb2.Max.Z - d;
			}
			if(pos1.Z > pos2.Z + bb2.Max.Z) {
				z = pos2.Z + bb2.Max.Z - bb2.Min.Z + d;
			}
			
			control->setLeftRight(0.0f);
			control->setForwardBackward(0.0f);
			t.translation(x, 0.0f, z);
			model1->transform(t*r);
			
			std::cout << "seite..." << angle << " "<< asin(m.m02) << std::endl;
		}
	}
	else {
		std::cout << "jumppower" << control->getJumpPower() << std::endl;
	}
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
	float newHeight = coinMat.translation().Y - 0.5f;
	
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
