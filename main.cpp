#include "internal/general/general.h"
#include "internal/general/glfwWrapper.h"
#include "internal/general/imGuiWrapper.h"

glfwWrapper glfwW;

int main(int, char**)
{
    // Create GLFW context
    glfwW.createGLFWContext();
    if (glfwW.getWindow() == nullptr) return 1;

    // Create imGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    imGuiWrapper imGuiW = imGuiWrapper(io);
    imGuiW.createImGuiContext(
        glfwW.getWindow(), glfwW.getMainScale(), glfwW.getGlslVersion()
    );

    // Main loop
    while (!glfwWindowShouldClose(glfwW.getWindow()))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
        if (glfwGetWindowAttrib(glfwW.getWindow(), GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Rendering imGui below, functions have to be executed sequentially as such
        imGuiW.startImGuiRender();
        glfwW.renderGLFW();
        imGuiW.finishImGuiRender();

        glfwSwapBuffers(glfwW.getWindow());
    }

    return 0;
}
