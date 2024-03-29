#ifdef WIN32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#endif

#include <stdio.h>
#include "Application.h"
#include "FreeImage.h"

void PrintOpenGLVersion();
void getResolution(int* width, int* height);

int main () {
	FreeImage_Initialise();
	// Start GL context and O/S window using the GLFW helper library
	if (!glfwInit ()){
		fprintf (stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	
#ifdef __APPLE__
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	
	int WindowWidth = WINDOW_WIDTH;
	int WindowHeight = WINDOW_HEIGHT;
	
	GLFWwindow* window;
	
	if (FULLSCREEN) {
		getResolution(&WindowWidth, &WindowHeight);
		window = glfwCreateWindow(WindowWidth, WindowHeight, GAME_TITLE, glfwGetPrimaryMonitor(), NULL);
	}
	else {
		window = glfwCreateWindow(WindowWidth, WindowHeight, GAME_TITLE, NULL, NULL);
	}

	if (!window){
		fprintf (stderr, "ERROR: can not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent (window);
	
#if WIN32
	glewExperimental = GL_TRUE;
	glewInit();
#endif
	
	//PrintOpenGLVersion();
	
	{
		double lastTime=0;
		Application App(window);
		App.start();
		while (!glfwWindowShouldClose (window)){
			double now = glfwGetTime();
			double delta = now - lastTime;
			lastTime = now;
			// once per frame
			glfwPollEvents();
			App.update((float)delta);
			App.draw();
			glfwSwapBuffers (window);
		}
		App.end();
	}
	
	glfwTerminate();
	return 0;
}

void PrintOpenGLVersion() {
	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version = glGetString (GL_VERSION);
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n\n\n\n\n\n\n\n\n\n\n\n", version);
}

void getResolution(int* width, int* height) {
	const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	
	*width = vmode->width;
	*height = vmode->height;
}
