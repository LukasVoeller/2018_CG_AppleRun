//
//  Character.h
//  CGXcode
//
//  Created by Sandra Tieben on 26.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Character_h
#define Character_h

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "Constants.h"
#include "Model.h"
#include "MovingItem.h"
#include "PhongShader.h"
#include "ConstantShader.h"

class Character : public Model
{
public:
	Character();
	Character(const char* ModelFile, bool FitSize=false, float scale=1.0);
	virtual ~Character();
	void update(float dtime);
	//virtual void draw(const BaseCamera& Cam);
	
	/** Steuerung **/
	void steer( float ForwardBackward, float LeftRight);
	void steer3d(float forwardBackward, float leftRight, float jump);
	void aim( const Vector& Target );
	
	/** Setter **/
	void setHovering(bool hover);
	void setForwardBackward(float fb);
	void setLeftRight(float lr);
	void setJump(float jmp);
	void setPosZ (float z);
	void setIsInAir(bool newIsInAir);
	void setPalette(MovingItem* p) { palette = p;};
	
	
	/** Getter ***/
	Vector getLatestPosition();
	bool getIsInAir();
	float getJumpPower();
	const AABB& getBoundingBox() const { return BoundingBox; };
	float getForwardBackward() const { return forwardBackward; };
	float getLeftRight() const { return leftRight; };
	
	bool getHovering() const;
	MovingItem* getPalette() const { return palette; };
	
	
	/** Debug **/
	void printLatestPosition();
protected:
	//void calcBoundingBox(AABB& Box);
private:
	float leftRight = 0;
	float forwardBackward = 0;
	float jump = 0;
	float cannonAngle = 0;
	float time = 0;
	float jumpPower = JUMPPOWER;
	bool isInAir = false;
	Vector target;
	Vector position;
	AABB BoundingBox;
	
	//für update und so..
	float posZ = 0;
	
	//für Hovering
	bool isHovering = false; //für Palette
	MovingItem* palette;
	
};

#endif /* Character_h */
