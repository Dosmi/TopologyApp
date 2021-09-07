#pragma once

#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "Window.h"

class Application
{
public:
	Application();
	int run();


private:
	Window w1;
};

