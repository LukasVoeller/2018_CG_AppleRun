//
//  OutlineShader.h
//  CGXcode
//
//  Created by Sandra Tieben on 10.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef OutlineShader_h
#define OutlineShader_h

#include <stdio.h>
#include <string>
#include "PhongShader.h"


class OutlineShader: public PhongShader
{
public:
	OutlineShader(bool loadStatic=true);
//	OutlineShader(const std::string& vs, const std::string& fs);
	virtual ~OutlineShader();
	virtual void activate(const BaseCamera& Cam) const;
	void time(float t) { Time = t; }
	float time() const { return Time; }
protected:
	float Time;
	GLint NormalMat;
	GLint Timer;
	
};


#endif /* OutlineShader_h */
