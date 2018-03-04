//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Character.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "Constants.h"
#include "Model.h"
#include "MovingItem.h"
#include "PhongShader.h"
#include "ConstantShader.h"

Character::Character():pallet(NULL), standOn(NULL)
{
	position = Vector(0.0, 0.0, 0.0);
}

Character::~Character()
{
	delete character;
}

bool Character::loadModel(const char* file, float scaling)
{
    this->character = new Model(file, false, scaling);
    this->character->shader(this->pShader, false);
	
	this->BoundingBox = this->character->getBoundingBox();
	
	this->character->scaleBoundingBox(Vector(scaling, scaling, scaling));
	this->scaledBoundingBox = this->character->getScaledBoundingBox();
	
    return true;
}

//Steuerung mit Sprung
void Character::steer3d( float ForwardBackward, float LeftRight, float Jump)
{
    this->forwardBackward = ForwardBackward;
    this->leftRight = LeftRight;
    this->jump = Jump;
}

void Character::update(float dtime)
{
	Matrix CharacMat = this->transform();
	Matrix forwardMat, steeringMat, hoverMat;
	
	//Bewegung berechnen
	float fbFactor = this->forwardBackward * dtime;
	float lrFactor = this->leftRight * dtime;
	float jpFactor = this->jump * dtime;
	float translatZ = this->posZ * dtime;
	
	if(pallet != NULL) {
		// Höhe abhängig von der Palette
		int additionalJumpFactor = 5;
		jpFactor *= additionalJumpFactor;
		hoverMat = pallet->getLocalTransform();
		steeringMat.rotationY(CharacMat.left().Z+lrFactor);
		forwardMat = forwardMat.translation(CharacMat.translation().X+fbFactor, hoverMat.translation().Y+jpFactor, CharacMat.translation().Z+translatZ);
		CharacMat = forwardMat * steeringMat;
	}
	else {
		forwardMat.translation(fbFactor, jpFactor, translatZ);
		steeringMat.rotationY(lrFactor);
		CharacMat = CharacMat * forwardMat * steeringMat;
	}
	
	//Aktuelle Position in Vektor speichern
	this->position.X = CharacMat.m[12];
	this->position.Y = std::max(CharacMat.m[13], TERRAIN_HEIGHT);
	this->position.Z = CharacMat.m[14];
	
	this->character->transform(CharacMat);
	this->transform(CharacMat);
	
}

void Character::draw(const BaseCamera& Cam)
{
    this->character->draw(Cam);
}

Vector Character::getLatestPosition() {
    return this->position;
}

void Character::printLatestPosition() {
    std::cout << "X:" << this->position.X << " ";
    std::cout << "Y:" << this->position.Y << " ";
    std::cout << "Z:" << this->position.Z << std::endl;
}

bool Character::getIsInAir(){
    return this->isInAir;
}

void Character::setIsInAir(bool newIsInAir){
    this->isInAir = newIsInAir;
}

float Character::getJumpPower(){
    return this->jumpPower;
}

//(Kleinste) achsenausgerichtete Box berechnen, die das Modell komplett umschließt
void Character::calcBoundingBox(AABB& Box)
{
	Box = this->character->getBoundingBox();
}

void Character::setHovering(bool hover) {
	this->isHovering = hover;
}

bool Character::getHovering() const
{
	return this->isHovering;
}

void Character::setForwardBackward(float fb) {
	forwardBackward = fb;
}

void Character::setLeftRight(float lr) {
	leftRight = lr;
}

void Character::setJump(float jmp) {
	jump = jmp;
}

void Character::setPosZ(float z) {
	posZ = z;
}

