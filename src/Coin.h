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
	Vector getLatestPosition();
	void setLatestPosition(Vector pos);
	bool collected = false;
protected:
	Vector position;
};

#endif /* Coin_h */
