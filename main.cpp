#include "internal/general/general.h"
#include "internal/general/glfwWrapper.h"
#include "internal/general/imGuiWrapper.h"
#include "internal/general/openGlWrapper.h"

glfwWrapper glfwW;
openGlWrapper openGlW;

int main()
{
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

    // OpenGL
    openGlW.compileShaders();
    openGlW.createVertexBuffersAndAttributes();

    // Main loop
    while (!glfwWindowShouldClose(glfwW.getWindow()))
    {
        glfwW.resizeGLFW();
        openGlW.render();
        imGuiW.render();
        glfwW.swapBuffers();
    }

    // Cleanup
    openGlW.deAllcoate();
    return 0;
}
