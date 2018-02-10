//
//  DeathBlock.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 08.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include <stdio.h>
#include "DeathBlock.h"
#include "Constants.h"

DeathBlock::DeathBlock() {}

DeathBlock::DeathBlock(const char* ModelFile, bool FitSize, float scale) {
	this->scale = scale;
	bool ret = load(ModelFile, FitSize);
	if(!ret)
		throw std::exception();
}

void DeathBlock::update(float dtime) {
	std::cout << "deathblock" << std::endl;
	Matrix CoinMat = this->transform();
	Matrix forwardMat;
	
	//forwardMat.translation(0, height*dtime, 0);
	
	CoinMat = CoinMat * forwardMat;
	this->transform(CoinMat);
	
}

