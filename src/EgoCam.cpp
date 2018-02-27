//
//  EgoCam.cpp
//  CGXcode
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "EgoCam.h"

void EgoCam::update() {
	
}

const Matrix& EgoCam::getViewMatrix() const {
	return View;
}

const Matrix& EgoCam::getProjectionMatrix() const {
	return Proj;
}

Vector EgoCam::position() const {
	Matrix InvView = View;
	InvView.invert();
	
	return InvView.translation();
}

