//
//  GUIEvents.cpp
//  CGXcode
//
//  Created by Sandra Tieben on 05.02.18.
//  Copyright © 2018 Philipp Lensing. All rights reserved.
//

#include "GUIEvents.h"
#include "Constants.h"
#include "PhongShader.h"

GUIEvents::GUIEvents(){
	this->helpIsActive = false;	
	this->menu = new Model(ASSET_DIRECTORY "menu.fbx", false, 0.003);
	this->menu->shader(new PhongShader(), true);
}
GUIEvents::~GUIEvents(){}

void GUIEvents::update(GLFWwindow* pWindow, Camera* cam) {
	// Reference value for timer (frames)
	int actionTimeout = 10;
	
	// Dont do anything else if the action cooldown is not up
	if (actionTimer > 0) {
		actionTimer--;
		return;
	}
	if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		actionTimer = actionTimeout;
		this->helpIsActive = !this->helpIsActive;
	}
	
}

void GUIEvents::draw(BaseCamera* cam) {
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Matrix m;
	SimpleCamera c;
	
	c.setProjectionMatrix(cam->getProjectionMatrix());
	m = cam->getViewMatrix();
	
	//Menu
	c.setViewMatrix(m.orthographic(1.0f, 1.0f, 1.0f, 2.0f));
	
	// If the menu is active, we draw that and nothing else
	if (this->helpIsActive == true) {
		this->menu->draw(c);
		return;
	}
	
};

bool GUIEvents::changeHelpMenu() {
	this->helpIsActive = !this->helpIsActive;
	return this->helpIsActive;
}

