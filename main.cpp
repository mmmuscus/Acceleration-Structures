#include "general/general.h"
#include "general/glfwWrapper.h"
#include "general/imGuiWrapper.h"
#include "general/openGlWrapper.h"

#include "rayTracer/renderer.h"
#include "rayTracer/scene.h"

#include "accelerationStructures/BVH.h"

glfwWrapper glfwW;
openGlWrapper openGlW;
renderer rayTracer;
scene sc(glm::vec3(0.0f, 0.0f, -18.0f));

int main()
{
    glfwW.createGLFWContext();
    if (glfwW.getWindow() == nullptr) return 1;
    std::cout << "GLFW Context successfully created" << std::endl;

    // Create imGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    imGuiWrapper imGuiW = imGuiWrapper(io);
    imGuiW.createImGuiContext(
        glfwW.getWindow(), glfwW.getGlslVersion()
    );

    std::cout << "imGui Context successfully created" << std::endl;

    // OpenGL
    openGlW.compileShaders();
    openGlW.createVertexBuffersAndAttributes();
    openGlW.createScreenTexture();
    std::cout << "OpenGL Context successfully created" << std::endl;

    // Init scene
    sc.initScene();
    std::cout << "Scene successfully populated" << std::endl;

    // Build BVH
    BVH bvh;
    bvh.buildBVH();

    sc.render(bvh, true);

    // Main loop
    while (!glfwWindowShouldClose(glfwW.getWindow()))
    {
        glfwW.resizeGLFW();
        openGlW.render();
        imGuiW.render();
        glfwW.swapBuffers();

        sc.render(bvh, false);

        std::cout << "Scene successfully populated" << std::endl;
    }

    // Cleanup
    openGlW.deAllcoate();
    return 0;
}
