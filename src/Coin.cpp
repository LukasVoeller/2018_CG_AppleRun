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
	Matrix m;

	if(collected) {
		m = m.translation(0, height*dtime, 0);
	}
	else {
		m = m.translation(this->position.X, 0, this->position.Z);
		this->transform(m);
		return;
	}
	
	CoinMat = CoinMat * m;
	
	//Aktuelle Position in Vektor speichern
	this->position.X = CoinMat.m[12];
	this->position.Y = CoinMat.m[13];
	this->position.Z = CoinMat.m[14];
	
	this->transform(CoinMat);
	
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

