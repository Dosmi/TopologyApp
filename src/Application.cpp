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

#include "Curve.h"
#include "Surface.h"

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

        Shader basic_shader("res/shaders/basic.shader");
        ShapePointCloud shapePC;
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
                                         /* radius     */ 10.f,
                                         /* center     */ center_coordinate);                
                

                Curve circle = evalCircle(/* radius */ 0.5f,
                                          /* steps  */ onscreen_menus.points_to_generate);

                float* points = new float[circle.size() * DIMENSION];

                //std::cout << "torus.VV.size() = " << circle.size() << std::endl;

                int count = 0;
                for (int i = 0; i < circle.size() * DIMENSION; i += DIMENSION)
                {
                    points[i] = circle[count].V.x;
                    points[i + 1] = circle[count].V.y;
                    points[i + 2] = circle[count].V.z;

                    count++;
                }
                shapePC.point_array = points;



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

                // profile circle:
                Curve profile = evalCircle(/* radius */ 0.5f,
                                           /* steps  */ onscreen_menus.points_to_generate);
                // sweep circle:
                Curve sweep = evalCircle(/* radius */ 2.f,
                                         /* steps  */ onscreen_menus.points_to_generate);


                Surface torus = makeGenCyl(profile, sweep, false);
                //shapePC.point_array = NULL;
                //shapePC.point_array = new float[torus.VV.size() * DIMENSION];
                //shapePC.index_array = indices;
                //num_points = points_to_generate;
                //num_elements = element_count;

                float* points = new float[torus.VV.size() * DIMENSION];
                unsigned* indices = new unsigned[torus.VF.size() * DIMENSION];

                int count = 0;
                for (int i = 0; i < torus.VV.size() * DIMENSION; i+= DIMENSION)
                {
                    points[i] = torus.VV[count].x;
                    points[i+1] = torus.VV[count].y;
                    points[i+2] = torus.VV[count].z;

                    count++;
                }
                count = 0;
                for (int i = 0; i < torus.VF.size() * DIMENSION; i += DIMENSION)
                {
                    indices[i] = torus.VF[count][0];
                    indices[i + 1] = torus.VF[count][1];
                    indices[i + 2] = torus.VF[count][2];

                    count++;
                }

                shapePC.point_array = points;
                shapePC.index_array = indices;

            }


            // Define vertex buffer:
            VertexArray va;
            VertexBuffer vb(/* data */ shapePC.point_array, /* size */ shapePC.num_elements * sizeof(float));
            VertexBufferLayout layout;
            layout.push<float>(DIMENSION);
            va.addBuffer(vb, layout);

            // Define index buffer:
            IndexBuffer ib(/* data */ shapePC.index_array, /* count */ shapePC.num_elements);


            basic_shader.bind();
            /* set the colour of the objects we are about to draw (as 4 float uniform RGBA): */
            basic_shader.setUniform4f("u_color", red_channel, 0.3f, 0.8f, 1.0f);

            ModelViewProjection MVP = w1.getCameraMatricesMVP();
            /* we now set the camera uniforms: */
            basic_shader.setUniformMat4f("u_model", MVP.model_matrix); // arcball_rot* look_at);
            basic_shader.setUniformMat4f("u_view", MVP.view_matrix); // arcball_rot* look_at);
            basic_shader.setUniformMat4f("u_projection", MVP.projection_matrix);

            




            /* update colour: */
            if (red_channel > 1.0f)
                increment = -0.005f;
            else if (red_channel < 0.0f)
                increment = 0.005f;
            red_channel += increment;
            
            /* with all options defined above (state-machine), we make the drawcall: */
            renderer.draw(va, ib, basic_shader);

            GLfloat pointVertex[] = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

            /* Render the GUI menu (the layout is defined in the member function) */
            onscreen_menus.renderPredefinedLayout();

            /* Swap front and back buffers */
            glfwSwapBuffers(w1.window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    onscreen_menus.shutdown();
    glfwTerminate();

    return 0;
}

