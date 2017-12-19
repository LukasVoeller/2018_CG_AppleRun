//
//  Tank.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Tank_hpp
#define Tank_hpp

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Model.h"
#include "PhongShader.h"
#include "ConstantShader.h"

class Tank : public BaseModel
{
public:
    Tank();
    virtual ~Tank();
    bool loadModels(const char* ChassisFile, const char* CannonFile);
    void steer( float ForwardBackward, float LeftRight);
    void steer3d(float forwardBackward, float leftRight, float jump);
    void aim( const Vector& Target );
    void update(float dtime);
    virtual void draw(const BaseCamera& Cam);
    Vector getLatestPosition();
protected:
private:
    Model* chassis;
    Model* cannon;
    float leftRight = 0;
    float forwardBackward = 0;
    float jump = 0;
    float cannonAngle = 0;
    Vector target;
    float time = 0;
};

#endif /* Tank_hpp */
