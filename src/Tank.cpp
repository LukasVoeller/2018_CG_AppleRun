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
    // TODO: Add your code
}

Tank::~Tank()
{
    // TODO: Add your code
    delete this->chassis;
    delete this->cannon;
}

bool Tank::loadModels(const char* ChassisFile, const char* CannonFile)
{
    // TODO: Add your code
    std::cout << "Load models" << std::endl;
    this->chassis = new Model(ChassisFile, false);
    this->cannon = new Model(CannonFile, false);
    
    this->chassis->shader(this->pShader, false);
    this->cannon->shader(this->pShader, false);
    
    return true;
}

/* Steuerung */
void Tank::steer( float ForwardBackward, float LeftRight)
{
    this->forwardBackward = ForwardBackward;
    this->leftRight = LeftRight;
}

/* Steuerung mit Sprung*/
void Tank::steer3d( float ForwardBackward, float LeftRight, float Jump)
{
    this->forwardBackward = ForwardBackward;
    this->leftRight = LeftRight;
    this->jump = Jump;
}


/* Zielen */
void Tank::aim(const Vector& Target )
{
    
    /*
     if (!(this->transform().translation() == Target)) {
     this->cannonAngle = this->transform().translation().angle(Target);
     } */
    
    this->target = Target;
}

void Tank::update(float dtime)
{
    
    Matrix TankMat = this->transform();
    
    // Panzerbewegung berechnen
    float fwFactor = forwardBackward * dtime;
    float lrFactor = leftRight * dtime;
    float jmpFactor = jump; // * dtime;

    
    Matrix forwardMat, steeringMat;
    forwardMat.translation(fwFactor, jmpFactor, 0);
    steeringMat.rotationY(lrFactor);
    
    TankMat = TankMat * forwardMat * steeringMat;
    
    // Kanone berechnen
    Matrix CannonMat;
    CannonMat.translation(TankMat.translation());
    
    Matrix tmp = CannonMat.lookAt(target, Vector(0, 1, 0), CannonMat.translation()).invert();
    CannonMat = tmp * CannonMat.rotationY(0.5 * M_PI);
    
    //Erlaubten Bereich definieren
    if(TankMat.m03 > -1.2f && TankMat.m03 < 4.0f)
    {
        // Alles zusammen bauen
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
    return Vector(0, 0, 0);
}

