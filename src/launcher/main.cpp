
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "main.h"

int display_w, display_h;

static void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    //
    // this is a very cursed way to handle this, this should be called
    // viewer interface-side!
    //
    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        viewer.MouseButtonCallback(window, button, action, mods);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    viewer.CursorPosCallback(window, xpos, ypos);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    viewer.KeyCallback(window, key, scancode, action, mods);
}

int main(int argc, char** argv)
{
    //
    // setup glfw window
    //
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
        return 1;

    //
    // set glfw flags here
    //
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "shinola's model viewer", NULL, NULL);

    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // enable vsync

    //
    // setup imgui display (and override io functions)
    //
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantSaveIniSettings = false;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    //
    // load the model viewer interface
    //
    if (argc < 2)
        return 1;

    viewer.Init(argv[1]);

    //
    // main loop
    //
    while (!glfwWindowShouldClose(window))
    {
        //
        // poll + handle events glfw-side
        // we can also do this imgui-side, but it's easier for glfw to take hold
        //
        glfwPollEvents();

        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetKeyCallback(window, KeyCallback);

        //
        // start the main imgui frame
        //
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //
        // draw the main display, then setup imgui rendering
        //
        disp_main.Render();
        ImGui::Render();

        //
        // init the opengl stuff
        //
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(disp_main.clear_color.x, disp_main.clear_color.y, disp_main.clear_color.z, disp_main.clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //
        // render the important things
        // - model viewer interface
        // - imgui (for real this time)
        //
        viewer.Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        //
        // ...and done!
        //
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    //
    // shutdown imgui + glfw
    //
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
