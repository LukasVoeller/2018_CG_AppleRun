//
//  Coin.h
//  CGXcode
//
//  Created by Sandra Tieben on 07.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef Coin_h
#define Coin_h

#include "Constants.h"
#include <stdio.h>
#include "Model.h"
#include <string>

class Coin : public Model
{
public:
	Coin();
	Coin(const char* ModelFile, bool FitSize=true, float scale=1.0);
	void update(float dtime);
	float getHeight();
	void setHeight(float height);
	Vector getLatestPosition();
	bool collected = false;
protected:
	float height = -30.0f;
	Vector position;
};

#endif /* Coin_h */