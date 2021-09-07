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
#include "ShapePointCloud.h"
#include "AppGUI.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"  // make_mat4 
#include "glm/gtx/string_cast.hpp" // for printing

// ImGUI imports:
//#include "imgui/imgui.h" 
//#include "imgui/imgui_impl_opengl3.h" 
//#include "imgui/imgui_impl_glfw.h"

#include <GL/glew.h>   
// NOTE: glew.h requires preprocessor definition GLEW_STATIC,
//       ... and needs to be imported before other gl headers
#include <GLFW/glfw3.h>

#include <gl/gl.h>
#include <gl/glu.h>


#define DIMENSION 3
#define SCREEN_WIDTH  1280 // 640
#define SCREEN_HEIGHT 720 // 480



Application::Application()
    :  w1()
{
    if (glewInit() != GLEW_OK)
    {
        std::cout << "'glewInit' did not return OK" << std::endl;
    }
    else
    {
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    }

}

int Application::run()
{
    AppGUI onscreen_menus(w1.window);
    {
        glm::vec3 center_coordinate(0.f, 0.f, 0.f);

        ShapePointCloud shapePC;
        shapePC.generateTorusPC(/* num_points */ 35,
            /* random?    */ false,
            /* radius     */ 1.f,
            /* center     */ center_coordinate);
        std::cout << shapePC.point_array[0] << ", " << shapePC.point_array[1] << " " << shapePC.point_array[2] << std::endl;
        // Define vertex buffer:
        VertexArray va;
        VertexBuffer vb(/* data */ shapePC.point_array, /* size */ shapePC.num_elements * sizeof(float));
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
        IndexBuffer ib(/* data */ shapePC.index_array, /* count */ shapePC.num_elements);

        glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f),
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
            0.1f, 100.f);

        glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f));
        glm::mat4 model_matrix = glm::mat4(1.0f); // leave the object at origin

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



        float red_channel = 0.0f;
        float increment = 0.05f;
        glm::vec3 translation(0.f, 0.f, 0.f);
        float rotation_radians = 0.0f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(w1.window))
        {

            /* Render here */
            renderer.clear();

            if (onscreen_menus.show_circle)
            {
                shapePC.generateCirclePC(/* num_points */ onscreen_menus.points_to_generate,
                    /* random?    */ false,
                    /* radius     */ 1.f,
                    /* center     */ center_coordinate);
            }
            else if (onscreen_menus.show_sphere)
            {
                shapePC.generateSpherePC(/* radius */ 1.f,
                    /* squish_stretch_factor */ 1.f,
                    /* steps 1               */ onscreen_menus.points_to_generate,
                    /* steps 2               */ onscreen_menus.points_to_generate);
            }
            else if (onscreen_menus.show_torus)
            {
                shapePC.generateTorusPC(/* num_points */ onscreen_menus.points_to_generate,
                    /* random?    */ false,
                    /* radius     */ 1.f,
                    /* center     */ center_coordinate);
            }


            // Define vertex buffer:
            VertexArray va;
            VertexBuffer vb(/* data */ shapePC.point_array, /* size */ shapePC.num_elements * sizeof(float));
            VertexBufferLayout layout;
            layout.push<float>(DIMENSION);
            va.addBuffer(vb, layout);

            // Define index buffer:
            IndexBuffer ib(/* data */ shapePC.index_array, /* count */ shapePC.num_elements);

            onscreen_menus.newFrame();

            //// IMGUI:
            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            //ImGui::NewFrame();
            //// :end IMGUI

            // IMGUI Control matrix:
            HMatrix rot;
            Qt_ToMatrix(w1.m_arcball.qNow, &rot);
            //debugCamera(rot);
            glm::mat4 arcball_rot = glm::make_mat4((float*)rot);


            // rotate object using quaternion rotations:
            glMultMatrixf((float*)w1.m_arcball.mNow);

            glm::vec3 eyepos(0.f, 0.f, w1.m_arcball.current_distance);
            glm::vec3 lookatpos(0.f, 0.f, 0.f);
            glm::vec3 upvect(0.f, 1.f, 0.f);

            glm::vec3 campos = arcball_rot * glm::vec4(eyepos, 1.f);


            // ------------------ Setting up the camera coordinate frame ------------------ //
            glm::vec3 forward_direction = glm::normalize(lookatpos - campos);
            // instead of using static right vector for rotations ...
            // (which alone causes one rotation axis locking up) ...
            // ... we rotate the 'right' vector together with the camera:
            glm::vec3 right_direction = glm::vec3(-1.0, 0.0, campos.z);
            right_direction = glm::vec3((arcball_rot * glm::vec4(right_direction, 1.0)));//.xyz;

            // compute 'up direction' as cross product beween the right and forward directions, ...
            // ... note, that 'right direction' is then recomputed
            glm::vec3 up_direction = glm::normalize(glm::cross(right_direction, forward_direction));
            right_direction = glm::normalize(glm::cross(campos, up_direction));

            float zoom = 0.9;

            //vec3 camera = campos + zoom * forward_direction;

            // ------------------ End of setting up the camera coordinate frame ------------------ //



            glm::mat4 model_matrix = glm::mat4(1.0f);

            glm::mat4 model_view_projection = projection_matrix * view_matrix * model_matrix;
            // :end IMGUI
            glm::mat4 look_at = glm::lookAt(/* Position of the camera                  */
                campos,
                /* Position where the camera is looking at */
                lookatpos,
                /* Normalized up vector, how the camera is oriented. Typically (0, 0, 1) */
                up_direction);
            //upvect);

    //std::cout << glm::to_string(eyepos)<< std::endl;
    //std::cout << glm::to_string(lookatpos)<< std::endl;
    //std::cout << glm::to_string(upvect)<< std::endl;
    //std::cout << glm::to_string(look_at)<< std::endl;

            basic_shader.bind();
            basic_shader.setUniform4f("u_color", red_channel, 0.3f, 0.8f, 1.0f);
            // we will be separating the model-view-projection matrix into:
            // - modelview
            // - projection
            // ... therefore, instead of setting this one uniform:
            //basic_shader.setUniformMat4f("u_modelviewprojection", model_view_projection);
            // we now set two:
            //basic_shader.setUniformMat4f("u_rotation_matrix", arcball_rot);
            //basic_shader.setUniformMat4f("u_modelview", view_matrix * model_matrix);
            basic_shader.setUniformMat4f("u_model", model_matrix); // arcball_rot* look_at);
            basic_shader.setUniformMat4f("u_view", look_at); // arcball_rot* look_at);
            basic_shader.setUniformMat4f("u_projection", projection_matrix);

            //basic_shader.setUniform3f("u_camera_position", 0.0, 0.0, m_arcball.current_distance);
            //basic_shader.setUniform3f("u_looking_at",      0.0, 0.0, 0.0);
            //basic_shader.setUniformMat4f("u_looking_at_matrix", look_at);
            //basic_shader.setUniformMat4f("u_camera_rotation", arcball_rot);

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

            onscreen_menus.renderPredefinedLayout();

            //// ImGUI Layout:
            //{
            //    static float f = 0.0f;
            //    ImGui::Begin("Hello, world!");
            //    ImGui::Text("Toggle Shape Options");               // Display some text (you can use a format strings too)
            //    ImGui::Checkbox("Show Circle", &show_circle);      // Edit bools storing our window open/close state
            //    ImGui::Checkbox("Show Sphere", &show_sphere);
            //    ImGui::Checkbox("Show Torus", &show_torus);

            //    ImGui::SliderFloat3("Translation", &translation.x, -500.f, 500.f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //    ImGui::SliderFloat("Rotate", &rotation_radians, -10.f, 10.f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //    ImGui::SliderInt("Points to Generate", &points_to_generate, 1, 1000);            // Edit 1 float using a slider from 0.0f to 1.0f

            //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            //    ImGui::End();
            //}


            //// IMGUI Render:
            //ImGui::Render();
            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            //// :end IMGUI

            /* Swap front and back buffers */
            glfwSwapBuffers(w1.window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }


    onscreen_menus.shutdown();
    //// IMGUI Shutdown:
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
    //// :end IMGUI
    glfwTerminate();

    return 0;
}

