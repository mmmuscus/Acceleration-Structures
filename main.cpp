#include "general/general.h"
#include "general/glfwWrapper.h"
#include "general/imGuiWrapper.h"
#include "general/openGlWrapper.h"

#include "rayTracer/renderer.h"

glfwWrapper glfwW;
openGlWrapper openGlW;
renderer rayTracer;

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
        glfwW.getWindow(), glfwW.getGlslVersion()
    );

    // OpenGL
    openGlW.compileShaders();
    openGlW.createVertexBuffersAndAttributes();
    openGlW.createScreenTexture();

    // Main loop
    while (!glfwWindowShouldClose(glfwW.getWindow()))
    {
        rayTracer.createScreenTexture();

        glfwW.resizeGLFW();
        openGlW.render();
        imGuiW.render();
        glfwW.swapBuffers();
    }

    // Cleanup
    openGlW.deAllcoate();
    return 0;
}
