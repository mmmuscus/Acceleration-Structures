#include "general/general.h"
#include "general/glfwWrapper.h"
#include "general/imGuiWrapper.h"
#include "general/openGlWrapper.h"

#include "rayTracer/scene.h"
#include "rayTracer/sceneGeometries.h"

#include "accelerationStructures/BVH.h"

glfwWrapper glfwW;
openGlWrapper openGlW;
scene sc(glm::vec3(0.0f, 0.0f, -18.0f));
sceneGeometries scGeos;

int main()
{
    // Init scene geometries
    scGeos.createBaseScene();
    scGeos.createTestScene();

    // Init scene
    sc.setSceneGeometry(scGeos.baseScene);
    std::cout << "Scene successfully populated" << std::endl;

    // List of BVHs
    BVH naiveBvh = BVH(NAIVE);
    BVH SAHBvh = BVH(SAH);
    BVH RDHBvh = BVH(RDH);
    BVH RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    BVH OHBvh = BVH(OH);

    // Build and serialize BVHs
    naiveBvh.buildAndSerialize("BVHs/naive.txt");
    //SAHBvh.buildAndSerialize("BVHs/SAH.txt");
    //RDHBvh.buildAndSerialize("BVHs/RDH-3-8.txt", sc.getCam(), 3, 8);
    //RDHSAHBlendBvh.buildAndSerialize("BVHs/RDHSAHBlended-3-8.txt", sc.getCam(), 3, 8);
    //OHBvh.buildAndSerialize("BVHs/OH-3.txt", sc.getCam(), 3);

    // Deserialiaze BVHs
    //naiveBvh.deserialize("BVHs/naive.txt");
    //SAHBvh.deserialize("BVHs/SAH.txt");
    //RDHBvh.deserialize("BVHs/RDH-3-8.txt");
    //RDHSAHBlendBvh.deserialize("BVHs/RDHSAHBlended-3-8.txt");
    //OHBvh.deserialize("BVHs/OH-3");
    
    // Create windows, and API contexts
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

    BVH& renderBvh = naiveBvh;

    // BVH render static
    /*
    float staticAngle = 270.0f;
    sc.spinCamera(staticAngle * M_PI / 180.0f);
    sc.render(renderBvh);
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
        sc.render(renderBvh);
        glfwW.resizeGLFW();
        openGlW.render();
        glfwW.swapBuffers();
    }

    printMCSV();

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
