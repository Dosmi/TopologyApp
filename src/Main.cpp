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

// ImGUI imports:
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "imgui/imgui_impl_glfw.h"


#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>

#define SCREEN_WIDTH  1280 // 640
#define SCREEN_HEIGHT 720 // 480

#define DIMENSION 3 

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
    std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

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

            std::cout << circle_x << ", " << circle_y << ", " << circle_z << std::endl;

            element_count += 3;
        }
    }

    return { points, indices, num_points, element_count };

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
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

        PointCloud circle = generateCirclePC(/* num_points */ 12, 
                                             /* random?    */ false,
                                             /* radius     */ 180.f,
                                             /* center     */ center_coordinate);

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
        int points_to_generate = 12;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.clear();

            PointCloud circle = generateCirclePC(/* num_points */ points_to_generate,
                /* random?    */ false,
                /* radius     */ 180.f,
                /* center     */ center_coordinate);

            // Define vertex buffer:
            VertexArray va;
            VertexBuffer vb(/* data */ circle.point_array, /* size */ circle.num_elements * sizeof(float));
            VertexBufferLayout layout;
            layout.push<float>(DIMENSION);
            va.addBuffer(vb, layout);

            // Define index buffer:
            //IndexBuffer ib(/* data */ indices, /* count */ 12);
            IndexBuffer ib(/* data */ circle.index_array, /* count */ circle.num_elements);


            // IMGUI:
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // :end IMGUI

            // IMGUI Control matrix:
            glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), translation);
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
            basic_shader.setUniformMat4f("u_modelview", view_matrix * model_matrix);
            basic_shader.setUniformMat4f("u_projection", projection_matrix);

            renderer.draw(va, ib, basic_shader);


            // draw a debug triangle (modern):
            //glDrawArrays(/* mode                        */ GL_TRIANGLES,
            //             /* starting idx                */ 0,
            //             /* number of indices to render */ 6);

            // update colour:
            if (red_channel > 1.0f)
                increment = -0.05f;
            else if (red_channel < 0.0f)
                increment = 0.05f;
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
                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

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
