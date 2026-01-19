#include "general/general.h"
#include "general/glfwWrapper.h"
#include "general/imGuiWrapper.h"
#include "general/openGlWrapper.h"

#include "rayTracer/scene.h"

#include "accelerationStructures/BVH.h"

glfwWrapper glfwW;
openGlWrapper openGlW;
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

    // Build BVHs
    BVH naiveBvh = BVH(NAIVE);
    naiveBvh.buildBVH();

    BVH SAHBvh = BVH(SAH);
    //SAHBvh.buildBVH(sc.getCam());

    BVH RDHBvh = BVH(RDH);
    RDHBvh.buildBVH(sc.getCam(), 3, 8);

    // BVH render static
    /*
    float staticAngle = 270.0f;
    sc.spinCamera(staticAngle * M_PI / 180.0f);
    sc.render(bvh, true);
    openGlW.setTextureIndex(3);
    glfwW.resizeGLFW();
    openGlW.render();
    glfwW.swapBuffers();
    */

    // BVH render spinning
    openGlW.setTextureIndex(3);
    for (angle = 0; angle < ANGLES; angle++) {
        std::cout << "Rendering scene with " << angle << " degree rotation around the y axis." << std::endl;
        sc.getCam()->spin((float)angle * M_PI / 180.0f);
        sc.render(RDHBvh, true);
        glfwW.resizeGLFW();
        openGlW.render();
        glfwW.swapBuffers();
    }

    printMCSV();
    
    // Normal render
    // intersection tests per pixel: number of triangles
    /*
    sc.render(bvh, false);
    glfwW.resizeGLFW();
    openGlW.render();
    glfwW.swapBuffers();
    */

    // Main loop
    while (!glfwWindowShouldClose(glfwW.getWindow()))
    {
        glfwW.resizeGLFW();
        openGlW.render();
        glfwW.swapBuffers();
    }

    // Cleanup
    openGlW.deAllcoate();
    return 0;
}
