//
//  Character.h
//	Lukas Voeller and Sandra Tieben
//	2018
//

#ifndef Character_hpp
#define Character_hpp

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Model.h"
#include "PhongShader.h"
#include "ConstantShader.h"

class Character : public BaseModel
{
public:
	Character();
	virtual ~Character();
	bool loadModels(const char* modelFile);
	void steer( float ForwardBackward, float LeftRight);
	void steer3d(float forwardBackward, float leftRight, float jump);
	void update(float dtime);
	virtual void draw(const BaseCamera& cam);
	Vector getLatestPosition();
	void printLatestPosition();
	bool getIsInAir();
	void setIsInAir(bool newIsInAir);
	float getJumpPower();
	const AABB& boundingBox() const { return BoundingBox; }
protected:
	void calcBoundingBox(AABB& Box);
private:
	Model* model;
	float leftRight = 0;
	float forwardBackward = 0;
	float jump = 0;
	float time = 0;
	float jumpPower = 15;
	bool isInAir = false;
	Vector position;
	AABB BoundingBox;
};

#endif /* Character_hpp */

