//
//  DeathBlock.h
//  CGXcode
//
//  Created by Sandra Tieben on 08.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef DeathBlock_h
#define DeathBlock_h

#include "Constants.h"
#include <stdio.h>
#include "Model.h"
#include <string>

class DeathBlock : public Model
{
public:
	DeathBlock();
	DeathBlock(const char* ModelFile, bool FitSize=true, float scale=1.0);
	void update(float dtime);
	
protected:
	
};

#endif /* DeathBlock_h */
