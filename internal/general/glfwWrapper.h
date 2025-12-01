#ifndef GLFW_WRAPPER_H_
#define GLFW_WRAPPER_H_

#include "general.h"

// Base is: https://github.com/ocornut/imgui/tree/docking/examples/example_glfw_opengl3
// Enhanced by: https://learnopengl.com/Getting-started/OpenGL
class glfwWrapper {
private:
    GLFWwindow* window;
    const char* glslVersion;
    float mainScale;

    static void glfwErrorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

public:
    glfwWrapper() : window(nullptr) {}

    void createGLFWContext() {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            return;

        // GL 3.0 + GLSL 130
        glslVersion = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window with graphics context
        mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
        window = glfwCreateWindow((int)(WIDTH * mainScale), (int)(HEIGHT * mainScale), "Acceleration Structures", nullptr, nullptr);
        if (window == nullptr)
            return;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            window = nullptr;
            return;
        }
    }

    void resizeGLFW() {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
    }

    void swapBuffers() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    const char* getGlslVersion() { return glslVersion; }
    float getMainScale() { return mainScale; }
    GLFWwindow* getWindow() { return window; }

    ~glfwWrapper() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

private:

};

#endif // !GLFW_WRAPPER_H_
