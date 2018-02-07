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

void Coin::update(float dtime) {
	// Reference value for timer (frames)
	Matrix CoinMat = this->transform();
	Matrix forwardMat;

//	float jpFactor = this->jump * dtime;
	
	//std::cout << fbFactor << " " << jpFactor <<std::endl;
	forwardMat.translation(0, -25*dtime, 0);
	
	CoinMat = CoinMat * forwardMat;
	
	//Matrix scaleMat;
	
	//irgendwas machen, wenn man dagegen springt
	//float scale = this->scale/2;
	
	//scaleMat.scale(scale);
	
	this->transform(CoinMat);
}
