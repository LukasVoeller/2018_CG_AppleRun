//
//  LoadPNG.hpp
//  CGXcode
//
//  Created by Lukas Völler on 19.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef LoadPNG_hpp
#define LoadPNG_hpp

#ifdef _WIN32
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <iostream>
#include <stdio.h>
#include "png.h"

GLubyte *textureImage;
float rotateX = 0;
float rotateY = 0;

int mouseX;
int mouseY;

class LoadPNG {
public:
	LoadPNG();
	~LoadPNG();
	
private:
	bool loadPNGImage(char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
	void init(void);
	void display(void);
	void myReshape(int w, int h);
	void mousePassive(int x, int y);
	void mouseMotion(int x, int y);

};

#endif /* LoadPNG_hpp */
