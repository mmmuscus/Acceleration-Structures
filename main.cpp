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

void spinBVHEval(BVH& renderBvh) {
    for (angle = 0; angle < ANGLES; angle++) {
        sc.getCam()->spin((float)angle * M_PI / 180.0f);
        sc.render(renderBvh);
        glfwW.resizeGLFW();
        openGlW.render();
        glfwW.swapBuffers();
    }

    printBVHEval();
}

int main()
{
    // Init scene geometries
    scGeos.createBaseScene();
    scGeos.createLowOcclusionSceneA();
    scGeos.createLowOcclusionSceneB();
    scGeos.createHighOcclusionSceneA();
    scGeos.createHighOcclusionSceneB();

    // List of BVHs
    BVH base_naive;
    base_naive.deserialize("BVHs/base_naive.txt");
    BVH base_SAH;
    base_SAH.deserialize("BVHs/base_SAH.txt");
    BVH base_RDH_6_4;
    base_RDH_6_4.deserialize("BVHs/base_RDH-6-4.txt");
    BVH base_RDH_6_5;
    base_RDH_6_5.deserialize("BVHs/base_RDH-6-5.txt");
    BVH base_RDH_6_10;
    base_RDH_6_10.deserialize("BVHs/base_RDH-6-10.txt");
    BVH base_RDH_10_12;
    base_RDH_10_12.deserialize("BVHs/base_RDH-10-12.txt");
    BVH base_RDH_20_20;
    base_RDH_20_20.deserialize("BVHs/base_RDH-20-20.txt");
    BVH base_RDHSAHBlend_6_2;
    base_RDHSAHBlend_6_2.deserialize("BVHs/base_RDHSAHBlend-6-2.txt");
    BVH base_RDHSAHBlend_6_4;
    base_RDHSAHBlend_6_4.deserialize("BVHs/base_RDHSAHBlend-6-4.txt");
    BVH base_RDHSAHBlend_6_10;
    base_RDHSAHBlend_6_10.deserialize("BVHs/base_RDHSAHBlend-6-10.txt");
    BVH base_RDHSAHBlend_10_12;
    base_RDHSAHBlend_10_12.deserialize("BVHs/base_RDHSAHBlend-10-12.txt");
    BVH base_RDHSAHBlend_20_20;
    base_RDHSAHBlend_20_20.deserialize("BVHs/base_RDHSAHBlend-20-20.txt");
    BVH base_OH_6;
    base_OH_6.deserialize("BVHs/base_OH-6.txt");
    BVH base_OH_10;
    base_OH_10.deserialize("BVHs/base_OH-10.txt");
    BVH base_OH_20;
    base_OH_20.deserialize("BVHs/base_OH-20.txt");

    BVH LOA_naive;
    LOA_naive.deserialize("BVHs/LOA_naive.txt");
    BVH LOA_SAH;
    LOA_SAH.deserialize("BVHs/LOA_SAH.txt");
    BVH LOA_RDH_6_4;
    LOA_RDH_6_4.deserialize("BVHs/LOA_RDH-6-4.txt");
    BVH LOA_RDH_6_10;
    LOA_RDH_6_10.deserialize("BVHs/LOA_RDH-6-10.txt");
    BVH LOA_RDH_10_12;
    LOA_RDH_10_12.deserialize("BVHs/LOA_RDH-10-12.txt");
    BVH LOA_RDH_20_20;
    LOA_RDH_20_20.deserialize("BVHs/LOA_RDH-20-20.txt");
    BVH LOA_RDHSAHBlend_6_2;
    LOA_RDHSAHBlend_6_2.deserialize("BVHs/LOA_RDHSAHBlend-6-2.txt");
    BVH LOA_RDHSAHBlend_6_4;
    LOA_RDHSAHBlend_6_4.deserialize("BVHs/LOA_RDHSAHBlend-6-4.txt");
    BVH LOA_RDHSAHBlend_6_10;
    LOA_RDHSAHBlend_6_10.deserialize("BVHs/LOA_RDHSAHBlend-6-10.txt");
    BVH LOA_RDHSAHBlend_10_12;
    LOA_RDHSAHBlend_10_12.deserialize("BVHs/LOA_RDHSAHBlend-10-12.txt");
    BVH LOA_RDHSAHBlend_20_20;
    LOA_RDHSAHBlend_20_20.deserialize("BVHs/LOA_RDHSAHBlend-20-20.txt");
    BVH LOA_OH_6;
    LOA_OH_6.deserialize("BVHs/LOA_OH-6.txt");
    BVH LOA_OH_10;
    LOA_OH_10.deserialize("BVHs/LOA_OH-10.txt");
    BVH LOA_OH_20;
    LOA_OH_20.deserialize("BVHs/LOA_OH-20.txt");

    BVH HOA_naive;
    HOA_naive.deserialize("BVHs/HOA_naive.txt");
    BVH HOA_SAH;
    HOA_SAH.deserialize("BVHs/HOA_SAH.txt");
    BVH HOA_RDH_6_4;
    HOA_RDH_6_4.deserialize("BVHs/HOA_RDH-6-4.txt");
    BVH HOA_RDH_6_10;
    HOA_RDH_6_10.deserialize("BVHs/HOA_RDH-6-10.txt");
    BVH HOA_RDH_10_12;
    HOA_RDH_10_12.deserialize("BVHs/HOA_RDH-10-12.txt");
    BVH HOA_RDH_20_20;
    HOA_RDH_20_20.deserialize("BVHs/HOA_RDH-20-20.txt");
    BVH HOA_RDHSAHBlend_6_2;
    HOA_RDHSAHBlend_6_2.deserialize("BVHs/HOA_RDHSAHBlend-6-2.txt");
    BVH HOA_RDHSAHBlend_6_4;
    HOA_RDHSAHBlend_6_4.deserialize("BVHs/HOA_RDHSAHBlend-6-4.txt");
    BVH HOA_RDHSAHBlend_6_10;
    HOA_RDHSAHBlend_6_10.deserialize("BVHs/HOA_RDHSAHBlend-6-10.txt");
    BVH HOA_RDHSAHBlend_10_12;
    HOA_RDHSAHBlend_10_12.deserialize("BVHs/HOA_RDHSAHBlend-10-12.txt");
    BVH HOA_RDHSAHBlend_20_20;
    HOA_RDHSAHBlend_20_20.deserialize("BVHs/HOA_RDHSAHBlend-20-20.txt");
    BVH HOA_OH_6;
    HOA_OH_6.deserialize("BVHs/HOA_OH-6.txt");
    BVH HOA_OH_10;
    HOA_OH_10.deserialize("BVHs/HOA_OH-10.txt");
    BVH HOA_OH_20;
    HOA_OH_20.deserialize("BVHs/HOA_OH-20.txt");


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

    // BVH render spinning
    openGlW.setTextureIndex(3);

    // base scene
    // Init scene
    sc.setSceneGeometry(scGeos.base);
    std::cout << std::endl << "!!! base Scene loaded" << std::endl;
    std::cout << "Evaulating base_naive" << std::endl;
    spinBVHEval(base_naive);
    std::cout << "Evaulating base_SAH" << std::endl;
    spinBVHEval(base_SAH);
    std::cout << "Evaulating base_RDH_6_4" << std::endl;
    spinBVHEval(base_RDH_6_4);
    std::cout << "Evaulating base_RDH_6_5" << std::endl;
    spinBVHEval(base_RDH_6_5);
    std::cout << "Evaulating base_RDH_6_10" << std::endl;
    spinBVHEval(base_RDH_6_10);
    std::cout << "Evaulating base_RDH_10_12" << std::endl;
    spinBVHEval(base_RDH_10_12);
    std::cout << "Evaulating base_RDH_20_20" << std::endl;
    spinBVHEval(base_RDH_20_20);
    std::cout << "Evaulating base_RDHSAHBlend_6_2" << std::endl;
    spinBVHEval(base_RDHSAHBlend_6_2);
    std::cout << "Evaulating base_RDHSAHBlend_6_4" << std::endl;
    spinBVHEval(base_RDHSAHBlend_6_4);
    std::cout << "Evaulating base_RDHSAHBlend_6_10" << std::endl;
    spinBVHEval(base_RDHSAHBlend_6_10);
    std::cout << "Evaulating base_RDHSAHBlend_10_12" << std::endl;
    spinBVHEval(base_RDHSAHBlend_10_12);
    std::cout << "Evaulating base_RDHSAHBlend_20_20" << std::endl;
    spinBVHEval(base_RDHSAHBlend_20_20);
    std::cout << "Evaulating base_OH_6" << std::endl;
    spinBVHEval(base_OH_6);
    std::cout << "Evaulating base_OH_10" << std::endl;
    spinBVHEval(base_OH_10);
    std::cout << "Evaulating base_OH_20" << std::endl;
    spinBVHEval(base_OH_20);

    sc.setSceneGeometry(scGeos.lowOcclusionA);
    std::cout << std::endl << "!!! LOA Scene loaded" << std::endl;
    std::cout << "Evaulating LOA_naive" << std::endl;
    spinBVHEval(LOA_naive);
    std::cout << "Evaulating LOA_SAH" << std::endl;
    spinBVHEval(LOA_SAH);
    std::cout << "Evaulating LOA_RDH_6_4" << std::endl;
    spinBVHEval(LOA_RDH_6_4);
    std::cout << "Evaulating LOA_RDH_6_10" << std::endl;
    spinBVHEval(LOA_RDH_6_10);
    std::cout << "Evaulating LOA_RDH_10_12" << std::endl;
    spinBVHEval(LOA_RDH_10_12);
    std::cout << "Evaulating LOA_RDH_20_20" << std::endl;
    spinBVHEval(LOA_RDH_20_20);
    std::cout << "Evaulating LOA_RDHSAHBlend_6_2" << std::endl;
    spinBVHEval(LOA_RDHSAHBlend_6_2);
    std::cout << "Evaulating LOA_RDHSAHBlend_6_4" << std::endl;
    spinBVHEval(LOA_RDHSAHBlend_6_4);
    std::cout << "Evaulating LOA_RDHSAHBlend_6_10" << std::endl;
    spinBVHEval(LOA_RDHSAHBlend_6_10);
    std::cout << "Evaulating LOA_RDHSAHBlend_10_12" << std::endl;
    spinBVHEval(LOA_RDHSAHBlend_10_12);
    std::cout << "Evaulating LOA_RDHSAHBlend_20_20" << std::endl;
    spinBVHEval(LOA_RDHSAHBlend_20_20);
    std::cout << "Evaulating LOA_OH_6" << std::endl;
    spinBVHEval(LOA_OH_6);
    std::cout << "Evaulating LOA_OH_10" << std::endl;
    spinBVHEval(LOA_OH_10);
    std::cout << "Evaulating LOA_OH_20" << std::endl;
    spinBVHEval(LOA_OH_20);

    sc.setSceneGeometry(scGeos.highOcclusionA);
    std::cout << std::endl << "!!! HOA Scene loaded" << std::endl;
    std::cout << "Evaulating HOA_naive" << std::endl;
    spinBVHEval(HOA_naive);
    std::cout << "Evaulating HOA_SAH" << std::endl;
    spinBVHEval(HOA_SAH);
    std::cout << "Evaulating HOA_RDH_6_4" << std::endl;
    spinBVHEval(HOA_RDH_6_4);
    std::cout << "Evaulating HOA_RDH_6_10" << std::endl;
    spinBVHEval(HOA_RDH_6_10);
    std::cout << "Evaulating HOA_RDH_10_12" << std::endl;
    spinBVHEval(HOA_RDH_10_12);
    std::cout << "Evaulating HOA_RDH_20_20" << std::endl;
    spinBVHEval(HOA_RDH_20_20);
    std::cout << "Evaulating HOA_RDHSAHBlend_6_2" << std::endl;
    spinBVHEval(HOA_RDHSAHBlend_6_2);
    std::cout << "Evaulating HOA_RDHSAHBlend_6_4" << std::endl;
    spinBVHEval(HOA_RDHSAHBlend_6_4);
    std::cout << "Evaulating HOA_RDHSAHBlend_6_10" << std::endl;
    spinBVHEval(HOA_RDHSAHBlend_6_10);
    std::cout << "Evaulating HOA_RDHSAHBlend_10_12" << std::endl;
    spinBVHEval(HOA_RDHSAHBlend_10_12);
    std::cout << "Evaulating HOA_RDHSAHBlend_20_20" << std::endl;
    spinBVHEval(HOA_RDHSAHBlend_20_20);
    std::cout << "Evaulating HOA_OH_6" << std::endl;
    spinBVHEval(HOA_OH_6);
    std::cout << "Evaulating HOA_OH_10" << std::endl;
    spinBVHEval(HOA_OH_10);
    std::cout << "Evaulating HOA_OH_20" << std::endl;
    spinBVHEval(HOA_OH_20);

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
