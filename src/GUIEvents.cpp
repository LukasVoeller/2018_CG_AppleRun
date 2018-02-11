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
	
	/* HelpMenu - Ein-/Ausblenden mit ESC */
	this->helpmenu = new Model(ASSET_DIRECTORY "menu.fbx", false, 0.003);
	this->helpmenu->shader(new PhongShader(), true);
	
	/* Ausblenden mit ENTER */
	this->startmenu = new Model(ASSET_DIRECTORY "menu.fbx", false, 0.003);
	this->startmenu->shader(new PhongShader(), true);
	
	/* Spielende */
	this->winningmenu = new Model(ASSET_DIRECTORY "menu.fbx", false, 0.003);
	this->winningmenu->shader(new PhongShader(), true);
}
GUIEvents::~GUIEvents(){}

void GUIEvents::update(GLFWwindow* pWindow, Camera* cam) {
	if (startIsActive && glfwGetKey(pWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
		this->startIsActive = false;
	}
	
	if (winningMenuIsActive && glfwGetKey(pWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
		this->winningMenuIsActive = false;
	}
	
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

	if(helpIsActive && glfwGetKey(pWindow, GLFW_KEY_P) == GLFW_PRESS) {
		//close Game
		this->closeWindow(pWindow);
		
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
	
	// Startmenu
	if (this->startIsActive == true) {
		this->startmenu->draw(c);
		return;
	}
	
	// Spielende
	if(this->winningMenuIsActive == true) {
		std::cout << "You won." << std::endl;
		this->winningmenu->draw(c);
		return;
	}
	
	// Helpmenu
	if (this->helpIsActive == true) {
		this->helpmenu->draw(c);
		return;
	}
	
};

bool GUIEvents::changeHelpMenu() {
	this->helpIsActive = !this->helpIsActive;
	return this->helpIsActive;
}

void GUIEvents::wonGame() {
	this->winningMenuIsActive = true;
}

void GUIEvents::restartGame() {
	this->startIsActive = true;
}

void GUIEvents::closeWindow(GLFWwindow* pWindow) {
	std::cout << "Bye bye" << std::endl;
	// Bricht while-Schleife in der main.cpp ab
	glfwSetWindowShouldClose(pWindow, 1);
}

