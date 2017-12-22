//
//  EgoCam.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 15.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef EgoCam_hpp
#define EgoCam_hpp

#include <stdio.h>
#include "Camera.h"

class EgoCam : public BaseCamera
{
public:
	EgoCam(GLFWwindow* pWin) : Window(pWin) {}
	virtual void update();
	virtual const Matrix& getViewMatrix() const;
	virtual const Matrix& getProjectionMatrix() const;
	virtual Vector position() const;
	Matrix& ViewMatrix() { return View; }
	Matrix& ProjMatrix() { return Proj; }
	virtual ~EgoCam() {};
	
protected:
	Matrix View;
	Matrix Proj;
	GLFWwindow* Window;
};

#endif /* EgoCam_hpp */
