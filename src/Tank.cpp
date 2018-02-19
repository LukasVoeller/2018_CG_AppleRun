//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"

Tank::Tank()
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
	BoundingBox = AABB::unitBox();
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
/*
    if (!(this->transform().translation() == Target)) {
        this->cannonAngle = this->transform().translation().angle(Target);
    }
*/
    this->target = Target;
}

void Tank::update(float dtime)
{
    Matrix TankMat = this->transform();
	Matrix forwardMat, steeringMat;
    
    //Panzerbewegung berechnen
    float fbFactor = this->forwardBackward * dtime;
    float lrFactor = this->leftRight * dtime;
    float jpFactor = this->jump * dtime;
	
	//std::cout << fbFactor << " " << jpFactor <<std::endl;
	forwardMat.translation(fbFactor, jpFactor, 0);

	steeringMat.rotationY(lrFactor);
    TankMat = TankMat * forwardMat * steeringMat;
    
    //Aktuelle Position in Vektor speichern
    this->position.X = TankMat.m[12];
    this->position.Y = std::max(TankMat.m[13], TERRAIN_HEIGHT);
    this->position.Z = TankMat.m[14];
    
    //Kanone berechnen
    Matrix CannonMat;
    CannonMat.translation(TankMat.translation());
    
    Matrix tmp = CannonMat.lookAt(target, Vector(0, 1, 0), CannonMat.translation()).invert();
    CannonMat = tmp * CannonMat.rotationY(0.5 * M_PI);
    
    //Erlaubten Bereich definieren
//    if(TankMat.m03 > -1.2f && TankMat.m03 < 4.0f)
	if(true)
    {
        //Alles zusammen bauen
        this->cannon->transform(CannonMat);
        this->chassis->transform(TankMat);
        this->transform(TankMat);
    }
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
	
	Box.min.X = std::min(chassisBox.min.X, cannonBox.min.X);
	Box.min.Y = std::min(chassisBox.min.Y, cannonBox.min.Y);
	Box.min.Z = std::min(chassisBox.min.Z, cannonBox.min.Z);

<<<<<<< HEAD
	Box.max.X = std::max(chassisBox.max.X, cannonBox.max.X);
	Box.max.Y = std::max(chassisBox.max.Y, cannonBox.max.Y);
	Box.max.Z = std::max(chassisBox.max.Z, cannonBox.max.Z);
	
	// Verschiebung der BoundingBox
	//	Box.Min.X = std::min(this->position.X - chassisBox.size().X/2, this->position.X - cannonBox.size().X/2);
	//	Box.Min.Y = std::min(this->position.Y - chassisBox.size().Y/2, this->position.Y - cannonBox.size().Y/2);
	//	Box.Min.Z = std::min(this->position.Z - chassisBox.size().Z/2, this->position.Z - cannonBox.size().Z/2);
	//
	//	Box.Min.X = std::max(this->position.X + chassisBox.size().X/2, this->position.X + cannonBox.size().X/2);
	//	Box.Min.Y = std::max(this->position.Y + chassisBox.size().Y/2, this->position.Y + cannonBox.size().Y/2);
	//	Box.Min.Z = std::max(this->position.Z + chassisBox.size().Z/2, this->position.Z + cannonBox.size().Z/2);

=======
	Box.Max.X = std::max(chassisBox.Max.X, cannonBox.Max.X);
	Box.Max.Y = std::max(chassisBox.Max.Y, cannonBox.Max.Y);
	Box.Max.Z = std::max(chassisBox.Max.Z, cannonBox.Max.Z);
>>>>>>> dce9ef01c80c7d3756e97d7bafc3ed8fd011e261
}
