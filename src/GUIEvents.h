//
//  GUIEvents.h
//  CGXcode
//
//  Created by Sandra Tieben on 05.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#ifndef GUIEvents_h
#define GUIEvents_h

#include "Model.h"
class GUIEvents
{
public:
	GUIEvents();
	~GUIEvents();
	bool changeHelpMenu();
	void wonGame();
	void restartGame();
	
	void update(GLFWwindow* pWindow, Camera* cam);
	void draw(BaseCamera* cam);
protected:
	Model* helpmenu = NULL;
	Model* startmenu = NULL;
	Model* winningmenu = NULL;
	
	bool helpIsActive = false;
	bool startIsActive = true;
	bool winningMenuIsActive = false;
	// timer for actions
	int actionTimer = 0;
};


#endif /* GUIEvents_h */

