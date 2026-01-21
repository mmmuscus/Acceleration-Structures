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
    scGeos.createLowOcclusionSceneA();
    scGeos.createLowOcclusionSceneB();
    scGeos.createHighOcclusionSceneA();
    scGeos.createHighOcclusionSceneB();

    // List of BVHs
    BVH naiveBvh = BVH(NAIVE);
    BVH SAHBvh = BVH(SAH);
    BVH RDHBvh = BVH(RDH);
    BVH RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    BVH OHBvh = BVH(OH);

    // naive and SAH for all scenes
    sc.setSceneGeometry(scGeos.base);
    naiveBvh.buildAndSerialize("BVHs/base_naive.txt");
    SAHBvh.buildAndSerialize("BVHs/base_SAH.txt");
    sc.setSceneGeometry(scGeos.lowOcclusionA);
    naiveBvh = BVH(NAIVE);
    SAHBvh = BVH(SAH);
    naiveBvh.buildAndSerialize("BVHs/LOA_naive.txt");
    SAHBvh.buildAndSerialize("BVHs/LOA_SAH.txt");
    sc.setSceneGeometry(scGeos.lowOcclusionB);
    naiveBvh = BVH(NAIVE);
    SAHBvh = BVH(SAH);
    naiveBvh.buildAndSerialize("BVHs/LOB_naive.txt");
    SAHBvh.buildAndSerialize("BVHs/LOB_SAH.txt");
    sc.setSceneGeometry(scGeos.highOcclusionA);
    naiveBvh = BVH(NAIVE);
    SAHBvh = BVH(SAH);
    naiveBvh.buildAndSerialize("BVHs/HOA_naive.txt");
    SAHBvh.buildAndSerialize("BVHs/HOA_SAH.txt");
    sc.setSceneGeometry(scGeos.highOcclusionB);
    BVH naiveBvh = BVH(NAIVE);
    BVH SAHBvh = BVH(SAH);
    naiveBvh.buildAndSerialize("BVHs/HOB_naive.txt");
    SAHBvh.buildAndSerialize("BVHs/HOB_SAH.txt");
    
    // OHs for important scenes
    sc.setSceneGeometry(scGeos.base);
    OHBvh.buildAndSerialize("BVHs/base_OH-6.txt", sc.getCam(), 6);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/base_OH-10.txt", sc.getCam(), 10);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/base_OH-20.txt", sc.getCam(), 20);
    sc.setSceneGeometry(scGeos.lowOcclusionA);
    OHBvh.buildAndSerialize("BVHs/LOA_OH-6.txt", sc.getCam(), 6);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/LOA_OH-10.txt", sc.getCam(), 10);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/LOA_OH-20.txt", sc.getCam(), 20);
    sc.setSceneGeometry(scGeos.highOcclusionA);
    OHBvh.buildAndSerialize("BVHs/HOA_OH-6.txt", sc.getCam(), 6);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/HOA_OH-10.txt", sc.getCam(), 10);
    OHBvh = BVH(OH);
    OHBvh.buildAndSerialize("BVHs/HOA_OH-20.txt", sc.getCam(), 20);

    // Important RDHs for important scenes
    sc.setSceneGeometry(scGeos.base);
    RDHBvh.buildAndSerialize("BVHs/base_RDH-6-1.txt", sc.getCam(), 6, 1);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/base_RDH-6-4.txt", sc.getCam(), 6, 4);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/base_RDH-10-2.txt", sc.getCam(), 10, 2);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/base_RDH-20-20.txt", sc.getCam(), 20, 20);
    RDHBvh = BVH(RDH);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/base_RDHSAHBlend-6-1.txt", sc.getCam(), 6, 1);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/base_RDHSAHBlend-6-4.txt", sc.getCam(), 6, 4);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/base_RDHSAHBlend-10-2.txt", sc.getCam(), 10, 2);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/base_RDHSAHBlend-20-20.txt", sc.getCam(), 20, 20);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    sc.setSceneGeometry(scGeos.lowOcclusionA);
    RDHBvh.buildAndSerialize("BVHs/LOA_RDH-6-1.txt", sc.getCam(), 6, 1);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/LOA_RDH-6-4.txt", sc.getCam(), 6, 4);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/LOA_RDH-10-2.txt", sc.getCam(), 10, 2);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/LOA_RDH-20-20.txt", sc.getCam(), 20, 20);
    RDHBvh = BVH(RDH);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/LOA_RDHSAHBlend-6-1.txt", sc.getCam(), 6, 1);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/LOA_RDHSAHBlend-6-4.txt", sc.getCam(), 6, 4);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/LOA_RDHSAHBlend-10-2.txt", sc.getCam(), 10, 2);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/LOA_RDHSAHBlend-20-20.txt", sc.getCam(), 20, 20);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    sc.setSceneGeometry(scGeos.highOcclusionA);
    RDHBvh.buildAndSerialize("BVHs/HOA_RDH-6-1.txt", sc.getCam(), 6, 1);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/HOA_RDH-6-4.txt", sc.getCam(), 6, 4);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/HOA_RDH-10-2.txt", sc.getCam(), 10, 2);
    RDHBvh = BVH(RDH);
    RDHBvh.buildAndSerialize("BVHs/HOA_RDH-20-20.txt", sc.getCam(), 20, 20);
    RDHBvh = BVH(RDH);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/HOA_RDHSAHBlend-6-1.txt", sc.getCam(), 6, 1);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/HOA_RDHSAHBlend-6-4.txt", sc.getCam(), 6, 4);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/HOA_RDHSAHBlend-10-2.txt", sc.getCam(), 10, 2);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);
    RDHSAHBlendBvh.buildAndSerialize("BVHs/HOA_RDHSAHBlend-20-20.txt", sc.getCam(), 20, 20);
    RDHSAHBlendBvh = BVH(RDHSAHBLEND);

    // Deserialiaze BVHs
    //naiveBvh.deserialize("BVHs/naive.txt");
    //SAHBvh.deserialize("BVHs/SAH.txt");
    //RDHBvh.deserialize("BVHs/RDH-3-8.txt");
    //RDHSAHBlendBvh.deserialize("BVHs/RDHSAHBlended-3-8.txt");
    //OHBvh.deserialize("BVHs/OH-3");

    // Init scene
    sc.setSceneGeometry(scGeos.highOcclusionB);
    std::cout << "Scene successfully populated" << std::endl;
    
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
