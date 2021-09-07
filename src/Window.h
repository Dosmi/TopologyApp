#pragma once

// Import Arcball Camera Class:
#include "camera/Arcball.h"
#include "ModelViewProjection.h"

#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>

class Window
{
public:
	Window();
	void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	ModelViewProjection getCameraMatricesMVP();

	GLFWwindow* window;
	int m_ScreenWidth, m_ScreenHeight;
	BallData m_arcball;
	bool m_mouse_pressed;
	bool m_invert_x = true;
//private:
};

