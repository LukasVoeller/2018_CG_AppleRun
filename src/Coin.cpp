//
//  Coin.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 06.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include "Coin.h"
#include "Constants.h"

Coin::Coin() {}

Coin::Coin(const char* ModelFile, bool FitSize, float scale) {
	this->scale = scale;
	bool ret = load(ModelFile, FitSize);
	if(!ret)
		throw std::exception();
}


Vector Coin::getLatestPosition() {
	return this->position;
}

void Coin::setLatestPosition(Vector pos) {
	this->position = pos;
}

