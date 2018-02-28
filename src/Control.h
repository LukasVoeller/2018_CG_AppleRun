//
//  Control.h
//  Steuerung des Characters
//
//  Created by Sandra Tieben on 20.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Control_h
#define Control_h

#include "Constants.h"
#include "EgoCam.h"
#include "Character.h"

class Control {
	
public:
	Control();
	Control(GLFWwindow* pWin);
	~Control();
	
	/* Read keyboardinputs */
	float readForwardBackward();
	float readLeftRight();
	float readJump(Character* character);
	
	
	float getForwardBackward() const {return this->forwardBackward; };
	float getLeftRight()  const { return this->leftRight; };
	float getJumpPower() const { return this->jumpPower; };
	
	void setForwardBackward(float fb) { this->forwardBackward = fb; };
	void setLeftRight(float lr) { leftRight = lr; };
	void setJumpPower(float jmp) {jumpPower = jmp; };
	
	void handleJump(Character* character);
private:
	GLFWwindow* pWindow;
	
	float forwardBackward = 0.0f;
	float leftRight = 0.0f;
	float jumpPower = 0.0f;
};

#endif /* Control_h */
