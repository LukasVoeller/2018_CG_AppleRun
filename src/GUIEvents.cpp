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
	this->helpmenu = new Model(ASSET_DIRECTORY "menu/helpmenu/menu.fbx", false, 0.0022);
	this->helpmenu->shader(new PhongShader(), true);
	
	/* Ausblenden mit ENTER */
	this->startmenu = new Model(ASSET_DIRECTORY "menu/startmenu/menu.fbx", false, 0.0022);
	this->startmenu->shader(new PhongShader(), true);
	
	/* Spielende */
	this->winningmenu = new Model(ASSET_DIRECTORY "menu/winningmenu/menu.fbx", false, 0.0022);
	this->winningmenu->shader(new PhongShader(), true);
}

GUIEvents::~GUIEvents(){}

void GUIEvents::update(GLFWwindow* pWindow, EgoCam* egocam) {
	if (startIsActive && glfwGetKey(pWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
		this->startIsActive = false;
	}
	
	if (winningMenuIsActive && glfwGetKey(pWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
		this->winningMenuIsActive = false;
	}
	
	// Cooldown actions
	if (coolDownTimer > 0) {
		coolDownTimer--;
		return;
	}
	if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		coolDownTimer = 10;
		this->helpIsActive = !this->helpIsActive;
	}
	
	if(helpIsActive && glfwGetKey(pWindow, GLFW_KEY_P) == GLFW_PRESS) {
		// Close Game
		this->closeWindow(pWindow);
	}
}

void GUIEvents::draw(EgoCam* egocam) {
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Matrix m;
	SimpleCamera c;
	
	c.setProjectionMatrix(egocam->getProjectionMatrix());
	m = egocam->getViewMatrix();
	
	//Menu
	c.setViewMatrix(m.orthographic(1.0f, 1.0f, 1.0f, 2.0f));
	
	// Startmenu
	if (this->startIsActive == true) {
		//this->drawImage(c);
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

GLuint GUIEvents::loadBMP(const char* imagepath) {
	//printf("Reading image %s\n", imagepath);
	
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height, bpp;
	// Actual RGB data
	unsigned char * data;
	
	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file){
		printf("Image could not be opened\n"); return 0;
	}
	
	// If less than 54 byes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){
		printf("Not a correct BMP file\n");
		return false;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	if ( *(int*)&(header[0x1C])!=24 ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	
	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	bpp = 3;
	
	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54;
	
	// Create a buffer
	data = new unsigned char [imageSize];
	
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
	
	// Everything is in memory now, the file wan be closed
	fclose (file);
	
	// Swap Red and Blue component for each texel of the image
	unsigned char t;
	for (unsigned int i = 0; i < imageSize; i += 3 ){
		t = data[i];
		data[i] = data[i+2];
		data[i+2] = t;
	}
	
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,(bpp == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, data);
	
	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// Return the ID of the texture we just created
	return textureID;
}

void GUIEvents::drawImage() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->image);
	
	
	// Draw a textured quad
//	glBegin(GL_QUADS);
//	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
//	glTexCoord2f(0, 1); glVertex3f(0, 100, 0);
//	glTexCoord2f(1, 1); glVertex3f(100, 100, 0);
//	glTexCoord2f(1, 0); glVertex3f(100, 0, 0);
//	glEnd();
//	
//	glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//	
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//	
//	glMatrixMode(GL_MODELVIEW);
//	glEnd();
}

