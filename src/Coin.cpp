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
	std::cout << "update coin" << height << std::endl;
	Matrix CoinMat = this->transform();
	Matrix m, s;

	if(collected) {
		this->position.X = CoinMat.m[12];
		this->position.Y = 0;
		this->position.Z = CoinMat.m[14];
		
		m = m.translation(0, height*dtime, 0);
		this->transform(CoinMat*m);
	}
	else {
		s = s.scale(this->scale);
		m = m.translation(this->position.X, 0, this->position.Z);
		this->transform(m*s);
	}
	
}

float Coin::getHeight() {
	return height;
}

void Coin::setHeight(float h) {
	height = h;
}

Vector Coin::getLatestPosition() {
	return this->position;
}

