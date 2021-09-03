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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 

// ImGUI imports:
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "imgui/imgui_impl_glfw.h"

// Import Arcball Camera Class:
#include "camera/Arcball.h"


#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>

#define SCREEN_WIDTH  1280 // 640
#define SCREEN_HEIGHT 720 // 480

#define DIMENSION 3

BallData m_arcball;
bool m_mouse_pressed;


struct PointCloud
{
    float* point_array;
    unsigned int*   index_array;
    int    num_points;
    int    num_elements;
};

PointCloud generateCirclePC(int num_points, bool random, float radius, glm::vec3 center)
{
    // allocate array which will be returned with points:
    //float* points = (float*)alloca(num_points * sizeof(float));
    float*        points  = new float[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    float degree_step = 360.f / float(num_points);
    //std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

    int element_count = 0;
    for (int i = 0; i < num_points; i++)
    {
        if (random)
        {
            // TODO
        }
        else
        {
            float deg = 0.f + degree_step * i;
            //std::cout << "move deg:" << deg << std::endl;
            //float circle_x = center.x - radius * glm::sin(deg);
            float circle_x = center.x - radius * glm::sin((deg * glm::pi<float>()) / 180.f);
            //float circle_y = center.y - radius * glm::cos(deg);
            float circle_y = center.y - radius * glm::cos((deg * glm::pi<float>()) / 180.f);
            float circle_z = 0.f;

            points[element_count  ] = circle_x;
            points[element_count+1] = circle_y;
            points[element_count+2] = circle_z;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;

            //std::cout << circle_x << ", " << circle_y << ", " << circle_z << std::endl;

            element_count += 3;
        }
    }

    return { points, indices, num_points, element_count };

}

PointCloud generateTorusPC(int num_points, bool random, float radius, glm::vec3 center)
{
    // allocate array which will be returned with points:
    //float* points = (float*)alloca(num_points * sizeof(float));
    float* points = new float[num_points*num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points*num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    float degree_step = 360.f / float(num_points);
    //std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

    int element_count = 0;
    for (int i = 0; i < num_points; i++)
    {
        float deg_out = 0.f + degree_step * i;
        //std::cout << "move deg:" << deg << std::endl;
        //float circle_x = center.x - radius * glm::sin(deg);
        float center_x = center.x - radius * glm::sin((deg_out * glm::pi<float>()) / 180.f);
        //float circle_y = center.y - radius * glm::cos(deg);
        float center_y = center.y - radius * glm::cos((deg_out * glm::pi<float>()) / 180.f);
        float center_z = 0.f;

        center = glm::vec3(center_x, center_y, center_z);

        for (int j = 0; j < num_points; j++)
        {
            float deg_in = 0.f + degree_step * j;
            //std::cout << "move deg:" << deg << std::endl;
            //float circle_x = center.x - radius * glm::sin((deg * glm::pi<float>()) / 180.f);
            float circle_x = radius * glm::sin((deg_in * glm::pi<float>()) / 180.f);
            //float circle_y = center.y - radius * glm::cos((deg * glm::pi<float>()) / 180.f);
            float circle_y = radius * glm::cos((deg_in * glm::pi<float>()) / 180.f);
            float circle_z = 0.f;

            glm::vec3 torus_point = glm::rotateX(glm::vec3(circle_x, circle_y, circle_z), 90 * glm::pi<float>() / 180.f);
            torus_point = glm::rotateZ(torus_point, deg_out * glm::pi<float>() / 180.f);

            points[element_count    ] = center.x - torus_point.x;
            points[element_count + 1] = center.y - torus_point.y;
            points[element_count + 2] = torus_point.z;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;

            //std::cout << circle_x << ", " << circle_y << ", " << circle_z << std::endl;

            element_count += 3;
        }
    }

    return { points, indices, num_points, element_count };
}

PointCloud generateSpherePC(const float& radius,
                            const float& squish_stretch_factor, 
                            const int& steps1, const int& steps2)
{
    const float PI = 3.1459;
    float x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;

    int num_points = 0;
    for (float theta = 0; theta < 2 * PI; theta += 2 * PI / steps1)
    {
        for (float phi = 0; phi < 2 * PI; phi += 2 * PI / steps2)
        {
            num_points += 4; 
        }
    }

    float* points = new float[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));
    unsigned int* indices = new unsigned int[num_points * DIMENSION]; //(float*)alloca(num_points * DIMENSION * sizeof(float));

    int element_count = 0;
    int point_count = 0;
    for (float theta = 0; theta < 2 * PI; theta += 2 * PI / steps1)
    {
        for (float phi = 0; phi < 2 * PI; phi += 2 * PI / steps2)
        {
            x0 = radius * cos(phi) * sin(theta);
            z0 = radius * sin(phi) * sin(theta);
            y0 = radius * cos(theta) * squish_stretch_factor;

            points[element_count] = x0;
            points[element_count + 1] = z0;
            points[element_count + 2] = y0;

            indices[element_count] = element_count;
            indices[element_count + 1] = element_count + 1;
            indices[element_count + 2] = element_count + 2;


            x1 = radius * cos(phi + 2 * PI / steps2) * sin(theta);
            z1 = radius * sin(phi + 2 * PI / steps2) * sin(theta);
            y1 = radius * cos(theta) * squish_stretch_factor;

            points[element_count + 3] = x1;
            points[element_count + 4] = z1;
            points[element_count + 5] = y1;

            indices[element_count + 3] = element_count + 3;
            indices[element_count + 4] = element_count + 4;
            indices[element_count + 5] = element_count + 5;



            x2 = radius * cos(phi) * sin(theta + 2 * PI / steps1);
            z2 = radius * sin(phi) * sin(theta + 2 * PI / steps1);
            y2 = radius * cos(theta + 2 * PI / steps1) * squish_stretch_factor;

            points[element_count + 6] = x2;
            points[element_count + 7] = z2;
            points[element_count + 8] = y2;

            indices[element_count + 6] = element_count + 6;
            indices[element_count + 7] = element_count + 7;
            indices[element_count + 8] = element_count + 8;



            x3 = radius * cos(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            z3 = radius * sin(phi + 2 * PI / steps2) * sin(theta + 2 * PI / steps1);
            y3 = radius * cos(theta + 2 * PI / steps2) * squish_stretch_factor;

            points[element_count + 9 ] = x3;
            points[element_count + 10] = z3;
            points[element_count + 11] = y3;

            indices[element_count + 9 ] = element_count + 9;
            indices[element_count + 10] = element_count + 10;
            indices[element_count + 11] = element_count + 11;

            element_count += 12;
            point_count += 4;
  
        }
    }

    return { points, indices, point_count, element_count };
}


void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    std::cout << button << "/" << action << std::endl;

    double x, y;
    //getting cursor position
    glfwGetCursorPos(window, &x, &y);
    std::cout << "Cursor Position at (" << x << " : " << y << std::endl;

    // if released mouse:
    if (m_mouse_pressed)
    {
        std::cout << "released ... " << x << " " << y << std::endl;
        Ball_EndDrag(&m_arcball);
        m_mouse_pressed = false;
        return;
    }

    else
    {
        m_arcball.mouse_start_x = x;
        m_arcball.mouse_start_y = y;
        HVect vNow;
        vNow.x = 2.f * (x - 0) / SCREEN_WIDTH - 1.f;
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
            break;
        default:
            break;
        }
    }
}


void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (m_mouse_pressed)
    {
        // write your drag code here`
        std::cout << "dragging ... " << xpos << " " << ypos << std::endl;
        Ball_Mouse(&m_arcball, xpos, ypos, SCREEN_WIDTH, SCREEN_HEIGHT);
        Ball_Update(&m_arcball);
    }
    else
    {   // just moving the mouse around ...
        return;
    }

}

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
    GLFWwindow* window;

    // Initialize the Windowing Library:
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    // (creates the OpenGL context here)
    glfwMakeContextCurrent(window);

    // sync with VSync (monitor's refresh rate)
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "'glewInit' did not return OK" << std::endl;
    }
    else
    {
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    }

    // set all the window mouse input callbacks for camera control:
    glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
    glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
    glfwSetCursorPosCallback(window, glfwCursorPosCallback);

    // 2. initialise camera parameters and camera:
    m_arcball.start_distance = 200.5f;
    m_arcball.current_distance = 200.5f;

    // initialise arcball camera:
    Ball_Init(&m_arcball);


    //glViewport(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    {
        // 2D point test:
        //float points[] = {
        // 100.f,  100.f,
        // 200.f,  100.f,
        // 200.f,  200.f,
        // 100.f,  200.f,
        //};
        
        glm::vec3 center_coordinate(500.f, 500.f, 0.f);

        //float points[] = {
        // 100.f,  100.f,  0.f,
        // //center_coordinate.x, center_coordinate.y, center_coordinate.z,
        // 200.f,  100.f,  0.f,
        // 200.f,  200.f,  0.f,
        // 100.f,  200.f,  0.f, 

        // 100.f,  100.f,  -200.f,
        // 200.f,  100.f,  -200.f,
        // 200.f,  200.f,  -200.f,
        // 100.f,  200.f,  -200.f,
        //};

        float points[] = {
         500.f,  320.f,  0.f,
         410.f,  344.f,  0.f,
         344.f,  410.f,  0.f,
         320.f,  500.f,  0.f, 

         344.f,  590.f,  0.f,
         410.f,  655.f,  0.f,
         500.f,  680.f,  0.f,
         590.f,  655.f,  0.f,

         655.f,  590.f,  0.f,
         680.f,  500.f,  0.f,
         655.f,  410.f,  0.f,
         590.f,  344.f,  0.f,
        };

        //PointCloud circle = generateCirclePC(/* num_points */ 12, 
        //                                     /* random?    */ false,
        //                                     /* radius     */ 180.f,
        //                                     /* center     */ center_coordinate);

        PointCloud circle = generateTorusPC(/* num_points */ 35,
            /* random?    */ false,
            /* radius     */ 180.f,
            /* center     */ center_coordinate);

        //PointCloud circle = generateSpherePC(/* radius */ 100.f,
        //                                     /* squish_stretch_factor */ 1.f,
        //                                     /* steps 1               */ 10,
        //                                     /* steps 2               */ 12);

        // Define vertex buffer:
        VertexArray va;
        VertexBuffer vb(/* data */ circle.point_array, /* size */ circle.num_elements * sizeof(float));
        //VertexBuffer vb(/* data */ points, /* size */ 12 * DIMENSION * sizeof(float));

        VertexBufferLayout layout;
        layout.push<float>(DIMENSION);
        va.addBuffer(vb, layout);

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4
        };

        unsigned int vao_id; // vertex array object ID
        GLCall(glGenVertexArrays(1, &vao_id));
        GLCall(glBindVertexArray(vao_id));

        // Define index buffer:
        //IndexBuffer ib(/* data */ indices, /* count */ 12);
        IndexBuffer ib(/* data */ circle.index_array, /* count */ circle.num_elements);

        // how the window is scaled (normalized device coordinates):
        //glm::mat4 projection_matrix = glm::ortho(/* left, right edges */ 0.f, 960.f, 
        //                                         /* top, bottom edges */ 0.f, 540.f, 
        //                                         /* near, far planes  */ 0.f, 1000.0f);
        glm::mat4 projection_matrix = glm::perspective(60.0f, 
                                                       (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                                       -0.1f, 1.f);

        glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f));
        //glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(200.f, 200.f, 0.f));

        //glm::mat4 model_view_projection = projection_matrix * view_matrix * model_matrix;

        Shader basic_shader("res/shaders/basic.shader");
        basic_shader.bind();
        basic_shader.setUniform4f("u_color", 0.2f, 0.3f, 0.8f, 1.0f);
        /*basic_shader.setUniformMat4f("u_modelviewprojection", model_view_projection);*/

        // Vertex Arrays in OpenGL:
        // UNBIND everything:
        va.unbind();
        //GLCall(glBindBuffer(/* type     */ GL_ARRAY_BUFFER,
        //                    /* ID       */ 0));
        vb.unbind();
        //GLCall(glBindBuffer(/* type     */ GL_ELEMENT_ARRAY_BUFFER,
        //                    /* ID       */ 0));
        ib.unbind();
        basic_shader.unbind();

        Renderer renderer;

        // set up ImGUI here:
        //Setup IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();                     // 1
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();                   // 3
        ImGui_ImplGlfw_InitForOpenGL(window, true); // 2
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        float red_channel = 0.0f;
        float increment = 0.05f;
        glm::vec3 translation(0.f, 0.f, 0.f);
        float rotation_radians = 0.0f;
        int points_to_generate = 35;

        // ImGUI show checkboxes:
        bool show_circle = false;
        bool show_sphere = false;
        bool show_torus = true;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {


            /* Render here */
            renderer.clear();

            //PointCloud circle = generateCirclePC(/* num_points */ points_to_generate,
            //    /* random?    */ false,
            //    /* radius     */ 180.f,
            //    /* center     */ center_coordinate);            
            

            if (show_circle)
            {
                //std::cout << "show_circle: " << show_circle;
                circle = generateCirclePC(/* num_points */ points_to_generate,
                                                     /* random?    */ false,
                                                     /* radius     */ 180.f,
                                                     /* center     */ center_coordinate);    
            }
            else if (show_sphere)
            {
                circle = generateSpherePC(/* radius */ 100.f,
                                                     /* squish_stretch_factor */ 1.f,
                                                     /* steps 1               */ points_to_generate,
                                                     /* steps 2               */ points_to_generate);
            }
            else if (show_torus)
            {
                circle = generateTorusPC(/* num_points */ points_to_generate,
                    /* random?    */ false,
                    /* radius     */ 180.f,
                    /* center     */ center_coordinate);
            }
            //PointCloud circle = generateTorusPC(/* num_points */ points_to_generate,
            //                                    /* random?    */ false,
            //                                    /* radius     */ 180.f,
            //                                    /* center     */ center_coordinate);


            // Define vertex buffer:
            VertexArray va;
            VertexBuffer vb(/* data */ circle.point_array, /* size */ circle.num_elements * sizeof(float));
            VertexBufferLayout layout;
            layout.push<float>(DIMENSION);
            va.addBuffer(vb, layout);

            // Define index buffer:
            IndexBuffer ib(/* data */ circle.index_array, /* count */ circle.num_elements);


            // IMGUI:
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // :end IMGUI

            // IMGUI Control matrix:
            HMatrix rot;
            Qt_ToMatrix(m_arcball.qNow, &rot);
            //debugCamera(rot);
            glm::mat4 arcball_rot = glm::make_mat4((float*)rot);




            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), translation);
            //glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), rot);
            model_matrix = glm::rotate(model_matrix, rotation_radians, glm::vec3(0.f, 1.f, 0.0f));

            glm::mat4 model_view_projection = projection_matrix * view_matrix * model_matrix;
            // :end IMGUI


            basic_shader.bind();
            basic_shader.setUniform4f("u_color", red_channel, 0.3f, 0.8f, 1.0f);
            // we will be separating the model-view-projection matrix into:
            // - modelview
            // - projection
            // ... therefore, instead of setting this one uniform:
            //basic_shader.setUniformMat4f("u_modelviewprojection", model_view_projection);
            // we now set two:
            basic_shader.setUniformMat4f("u_modelview", arcball_rot);
            //basic_shader.setUniformMat4f("u_modelview", view_matrix * model_matrix);
            basic_shader.setUniformMat4f("u_projection", projection_matrix);

            renderer.draw(va, ib, basic_shader);


            // draw a debug triangle (modern):
            //glDrawArrays(/* mode                        */ GL_TRIANGLES,
            //             /* starting idx                */ 0,
            //             /* number of indices to render */ 6);

            // update colour:
            if (red_channel > 1.0f)
                increment = -0.005f;
            else if (red_channel < 0.0f)
                increment = 0.005f;
            red_channel += increment;


            GLfloat pointVertex[] = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

            //// Test Draw Points:
            //glEnable(GL_POINT_SMOOTH);
            //    glEnableClientState(GL_VERTEX_ARRAY);
            //        glPointSize(50);
            //        glVertexPointer(/* size     */ 2,         // x, y = 2
            //                        /* type     */ GL_FLOAT,
            //                        /* stride   */ 0,         // single vertex, no colour
            //                        /* data_ptr */ pointVertex);
            //        glDrawArrays(/* drawing mode      */ GL_POINTS,
            //                     /* start on vertex # */ 0,
            //                     /* how many vertices */ 1);
            //    glDisableClientState(GL_VERTEX_ARRAY);
            //glEnable(GL_POINT_SMOOTH);

            // ImGUI Layout:
            {
                static float f = 0.0f;
                ImGui::Begin("Hello, world!");
                ImGui::Text("Toggle Shape Options");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Show Circle", &show_circle);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Show Sphere", &show_sphere);
                ImGui::Checkbox("Show Torus",  &show_torus);

                ImGui::SliderFloat3("Translation", &translation.x, -500.f, 500.f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderFloat("Rotate", &rotation_radians,-10.f, 10.f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderInt("Points to Generate", &points_to_generate, 1, 1000);            // Edit 1 float using a slider from 0.0f to 1.0f

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }


            // IMGUI Render:
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // :end IMGUI

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    // IMGUI Shutdown:
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // :end IMGUI

    glfwTerminate();
    return 0;
}
