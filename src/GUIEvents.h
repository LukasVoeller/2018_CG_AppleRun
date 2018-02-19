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
#include "EgoCam.h"
class GUIEvents
{
public:
	GUIEvents();
	~GUIEvents();
	bool changeHelpMenu();
	void wonGame();
	void restartGame();
	
	void update(GLFWwindow* pWindow, EgoCam* egocam);
	void draw(EgoCam* egocam);
protected:
	Model* helpmenu = NULL;
	Model* startmenu = NULL;
	Model* winningmenu = NULL;
	
	bool helpIsActive = false;
	bool startIsActive = true;
	bool winningMenuIsActive = false;
	// timer for actions
	int coolDownTimer = 0;
	
	void closeWindow(GLFWwindow* pWindow);
};


#endif /* GUIEvents_h */

