//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "Constants.h"
#include "Model.h"
#include "MovingItem.h"
#include "PhongShader.h"
#include "ConstantShader.h"

Tank::Tank():palette(NULL)
{

}

Tank::~Tank()
{
    delete this->chassis;
    delete this->cannon;
}

bool Tank::loadModels(const char* ChassisFile, const char* CannonFile)
{
    this->chassis = new Model(ChassisFile, false);
    this->cannon = new Model(CannonFile, false);
    
    this->chassis->shader(this->pShader, false);
    this->cannon->shader(this->pShader, false);
	
	/* unitBox() ist eine sehr schlechte Lösung für die BoundingBox
	 * Bessere wäre diese zu berechnen (aus Gehäuse und Kanone)
	 * Aber: Werte für Gehäuse und Kanone falsch (?), zumindest riesig
	 * daher erstmal die schlechte Lösung */
	AABB bb = AABB(Vector(-1.0f, 0, -1.0f), Vector(1.0f, 1.7f, 1.0f));
	BoundingBox = bb;
	//BoundingBox = AABB::unitBox();
	//calcBoundingBox(this->BoundingBox);
    
    return true;
}

//Steuerung
void Tank::steer( float ForwardBackward, float LeftRight)
{
    this->forwardBackward = ForwardBackward;
    this->leftRight = LeftRight;
}

//Steuerung mit Sprung
void Tank::steer3d( float ForwardBackward, float LeftRight, float Jump)
{
    this->forwardBackward = ForwardBackward;
    this->leftRight = LeftRight;
    this->jump = Jump;
}

//Zielen
void Tank::aim(const Vector& Target )
{
/*  if (!(this->transform().translation() == Target)) {
        this->cannonAngle = this->transform().translation().angle(Target);
    } */
    this->target = Target;
}

void Tank::update(float dtime)
{
	Matrix TankMat = this->transform();
	Matrix forwardMat, steeringMat, hoverMat;
	
	//Panzerbewegung berechnen
	float fbFactor = this->forwardBackward * dtime;
	float lrFactor = this->leftRight * dtime;
	float jpFactor = this->jump * dtime;
	float translatZ = this->posZ * dtime;
	
	
	if(palette != NULL) {
		//Höhe abhängig von der Palette
		int additionalJumpFactor = 5;
		jpFactor *= additionalJumpFactor;
		hoverMat = palette->getLocalTransform();
		steeringMat.rotationY(TankMat.left().Z+lrFactor);
		forwardMat = forwardMat.translation(TankMat.translation().X+fbFactor, hoverMat.translation().Y+jpFactor, TankMat.translation().Z+translatZ);
		TankMat = forwardMat * steeringMat;
	}
	else {
		forwardMat.translation(fbFactor, jpFactor, translatZ);
		steeringMat.rotationY(lrFactor);
		TankMat = TankMat * forwardMat * steeringMat;
	}
	
	//Aktuelle Position in Vektor speichern
	this->position.X = TankMat.m[12];
	this->position.Y = std::max(TankMat.m[13], TERRAIN_HEIGHT);
	this->position.Z = TankMat.m[14];

	//Kanone berechnen
	Matrix CannonMat;
	CannonMat.translation(TankMat.translation());

	Matrix tmp = CannonMat.lookAt(target, Vector(0, 1, 0), CannonMat.translation()).invert();
	CannonMat = tmp * CannonMat.rotationY(0.5 * M_PI);

	this->cannon->transform(CannonMat);
	this->chassis->transform(TankMat);
	this->transform(TankMat);
}

void Tank::draw(const BaseCamera& Cam)
{
    this->chassis->draw(Cam);
    this->cannon->draw(Cam);
}

Vector Tank::getLatestPosition() {
    return this->position;
}

void Tank::printLatestPosition() {
    std::cout << "X:" << this->position.X << " ";
    std::cout << "Y:" << this->position.Y << " ";
    std::cout << "Z:" << this->position.Z << std::endl;
}

bool Tank::getIsInAir(){
    return this->isInAir;
}

void Tank::setIsInAir(bool newIsInAir){
    this->isInAir = newIsInAir;
}

float Tank::getJumpPower(){
    return this->jumpPower;
}

//(Kleinste) achsenausgerichtete Box berechnen, die das Modell komplett umschließt
void Tank::calcBoundingBox(AABB& Box)
{
	const AABB& chassisBox = this->chassis->getBoundingBox();
	const AABB& cannonBox = this->cannon->getBoundingBox();
	
	Box.Min.X = std::min(chassisBox.Min.X, cannonBox.Min.X);
	Box.Min.Y = std::min(chassisBox.Min.Y, cannonBox.Min.Y);
	Box.Min.Z = std::min(chassisBox.Min.Z, cannonBox.Min.Z);

	Box.Max.X = std::max(chassisBox.Max.X, cannonBox.Max.X);
	Box.Max.Y = std::max(chassisBox.Max.Y, cannonBox.Max.Y);
	Box.Max.Z = std::max(chassisBox.Max.Z, cannonBox.Max.Z);
}

void Tank::setHovering(bool hover) {
	this->isHovering = hover;
}

bool Tank::getHovering() const
{
	return this->isHovering;
}

void Tank::setForwardBackward(float fb) {
	forwardBackward = fb;
}

void Tank::setLeftRight(float lr) {
	leftRight = lr;
}

void Tank::setJump(float jmp) {
	jump = jmp;
}

void Tank::setPosZ(float z) {
	posZ = z;
}
