#include "Window.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


// ImGUI imports:
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "imgui/imgui_impl_glfw.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing

// default screen width and heights
#define SCREEN_WIDTH  1280 // 640
#define SCREEN_HEIGHT 720 // 480

Window::Window()
    : m_ScreenWidth(SCREEN_WIDTH),
      m_ScreenHeight(SCREEN_HEIGHT)

{

    // Initialize the Windowing Library:
    if (!glfwInit())
    {
        std::cout << "Could not initialize glfw" << std::endl;
    }
    else
    {
        std::cout << "Initialised glfw successfully" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(m_ScreenWidth, m_ScreenHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout << "!window" << std::endl;
    }

    /* Make the window's context current */
    // (creates the OpenGL context here)
    glfwMakeContextCurrent(window);

    // sync with VSync (monitor's refresh rate)
    glfwSwapInterval(1);


    glfwSetWindowUserPointer(window, this);

    auto funcglfwMouseButtonCallback = [](GLFWwindow* w, int button, int action, int mods)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(w))->glfwMouseButtonCallback(w, button, action, mods);
    };

    auto funcglfwCursorPosCallback = [](GLFWwindow* window, double xpos, double ypos)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->glfwCursorPosCallback(window, xpos, ypos);
    };

    // set all the window mouse input callbacks for camera control:
    //glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);;;
    glfwSetMouseButtonCallback(window, funcglfwMouseButtonCallback);
    glfwSetCursorPosCallback(window, funcglfwCursorPosCallback);

    // 2. initialise camera parameters and camera:
    m_arcball.start_distance = 3.5f;
    m_arcball.current_distance = 3.5f;

    // initialise arcball camera:
    Ball_Init(&m_arcball);

    // set the arcball viewport:
    Ball_SetViewport(&m_arcball, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Window::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    //std::cout << button << "/" << action << std::endl;

    double x, y;
    //getting cursor position
    glfwGetCursorPos(window, &x, &y);
    //std::cout << "Cursor Position at (" << x << " : " << y << std::endl;

    // if released mouse:
    if (m_mouse_pressed)
    {
        //std::cout << "released ... " << x << " " << y << std::endl;
        //std::cout << "curr_dst:" << m_arcball.current_distance;
        Ball_EndDrag(&m_arcball);
        m_mouse_pressed = false;
        return;
    }

    else
    {
        if (m_invert_x)
        {
            m_arcball.mouse_start_x = glm::abs(SCREEN_WIDTH - x);//x;
        }
        else
        {
            m_arcball.mouse_start_x = x;
        }
        m_arcball.mouse_start_y = y;
        HVect vNow;
        if (m_invert_x)
        {
            vNow.x = 2.f * (glm::abs(SCREEN_WIDTH - x) - 0) / SCREEN_WIDTH - 1.f;
        }
        else
        {
            vNow.x = 2.f * (x - 0) / SCREEN_WIDTH - 1.f;
        }
        vNow.y = 2.f * (y - 0) / SCREEN_HEIGHT - 1.f;

        Ball_Mouse(&m_arcball, vNow);

        m_mouse_pressed = true;

        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            Ball_BeginDrag(&m_arcball, LEFT);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            Ball_BeginDrag(&m_arcball, RIGHT);
            m_arcball.current_distance = m_arcball.start_distance;
            //std::cout << "curr_dst:" << m_arcball.current_distance;
            break;
        default:
            break;
        }
    }
}


void Window::glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (m_mouse_pressed)
    {
        // write your drag code here`
        //std::cout << "dragging ... " << xpos << " " << ypos << std::endl;
        //std::cout << "dragging ... " << glm::abs(SCREEN_WIDTH - xpos) << " " << ypos << std::endl;
        if (m_invert_x)
        {
            Ball_Mouse(&m_arcball, glm::abs(SCREEN_WIDTH - xpos), ypos, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else
        {
            Ball_Mouse(&m_arcball, xpos, ypos, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        Ball_Update(&m_arcball);
    }
    else
    {   // just moving the mouse around ...
        return;
    }

}