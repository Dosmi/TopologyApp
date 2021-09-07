#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "Application.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing

// ImGUI imports:
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "imgui/imgui_impl_glfw.h"

// Import Arcball Camera Class:
#include "camera/Arcball.h"


//#include <GL/glew.h>   
//// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
////       ... and needs to be imported before other gl headers
//#include <GLFW/glfw3.h>
//
//#include <gl/gl.h>
//#include <gl/glu.h>

#define SCREEN_WIDTH  1280 // 640
#define SCREEN_HEIGHT 720 // 480

#define DIMENSION 3

//BallData m_arcball;
//bool m_mouse_pressed;
//bool m_invert_x = true;



void debugCamera(HMatrix rot)
{
    std::cout << "!camera-transform: \n"
        << "!< (0) [0][0] " << rot[0][0] << " (4) [0][1] " << rot[0][1] << " (8) [0][2] " << rot[0][2] << " (12) [0][3] " << rot[0][3] << ">\n"
        << "!< (1) [1][0] " << rot[1][0] << " (5) [1][1] " << rot[1][1] << " (9) [1][2] " << rot[1][2] << " (13) [1][3] " << rot[1][3] << ">\n"
        << "!< (2) [2][0] " << rot[2][0] << " (6) [2][1]" << rot[2][1] << " (10) [2][2] " << rot[2][2] << " (14) [2][3] " << rot[2][3] << ">\n"
        << "!< (3) [3][0] " << rot[3][0] << " (7) [3][1]" << rot[3][1] << " (11) [3][2] " << rot[3][2] << " (15) [3][3] " << rot[3][3] << ">\n]";
}

int main(void)
{

    Application topology_app;

    topology_app.run();
    

    return 0;
}
