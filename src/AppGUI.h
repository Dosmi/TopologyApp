#pragma once

#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>

class AppGUI
{
public:
	AppGUI(GLFWwindow* window);
	void newFrame();
	void renderPredefinedLayout();
	void shutdown();

	bool show_circle;
	bool show_sphere;
	bool show_torus;

	int points_to_generate;


};

