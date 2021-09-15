#include "AppGUI.h"

// ImGUI imports:
#include "imgui/imgui.h" 
#include "imgui/imgui_impl_opengl3.h" 
#include "imgui/imgui_impl_glfw.h"



AppGUI::AppGUI(GLFWwindow* window) 
    : show_circle(false),
      show_sphere(false),
      show_torus(true),
      points_to_generate(44)

{
    // set up ImGUI here:
    //Setup IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();                     // 1
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();                   // 3
    ImGui_ImplGlfw_InitForOpenGL(window, true); // 2
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
}


void AppGUI::renderPredefinedLayout()
{
    // IMGUI:
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // :end IMGUI

    // ImGUI Layout:
    {
        static float f = 0.0f;
        ImGui::Begin("Hello, world!");
        ImGui::Text("Toggle Shape Options");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Show Circle", &show_circle);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Show Sphere", &show_sphere);
        ImGui::Checkbox("Show Torus", &show_torus);

        //ImGui::SliderFloat3("Translation", &translation.x, -500.f, 500.f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::SliderFloat("Rotate", &rotation_radians, -10.f, 10.f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderInt("Points to Generate", &points_to_generate, 1, 100);            // Edit 1 float using a slider from 0.0f to 1.0f

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // IMGUI Render:
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // :end IMGUI
}

void AppGUI::shutdown()
{
    // IMGUI Shutdown:
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // :end IMGUI
}
