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
BVH bvh;

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

    // Guided by: https://vulkan-tutorial.com/Loading_models
    // Also by: https://github.com/canmom/rasteriser/blob/master/fileloader.cpp
    // Load model
    std::cout << "Loading model started" << std::endl;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string path = "assets/teapot.obj";

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(err);
    }

    std::vector<glm::vec3> vertices;

    // Load vertices
    for (const auto& shape : shapes) {
        const std::vector<tinyobj::index_t>& indices = shape.mesh.indices;
        const std::vector<int>& mat_ids = shape.mesh.material_ids;

        for (size_t face_ind = 0; face_ind < mat_ids.size(); face_ind++) {
            triangle prim;

            prim.p0 = glm::vec3(
                attrib.vertices[3 * indices[3 * face_ind].vertex_index + 0],
                attrib.vertices[3 * indices[3 * face_ind].vertex_index + 1],
                attrib.vertices[3 * indices[3 * face_ind].vertex_index + 2]
            );
            prim.p1 = glm::vec3(
                attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 0],
                attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 1],
                attrib.vertices[3 * indices[3 * face_ind + 1].vertex_index + 2]
            );
            prim.p2 = glm::vec3(
                attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 0],
                attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 1],
                attrib.vertices[3 * indices[3 * face_ind + 2].vertex_index + 2]
            );
            prim.calculateCentroid();

            prims.push_back(prim);
        }
    }

    // Scene
    //sc.initScene();
    std::cout << "Scene successfully populated" << std::endl;

    // Build BVH
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
    }

    // Cleanup
    openGlW.deAllcoate();
    return 0;
}
