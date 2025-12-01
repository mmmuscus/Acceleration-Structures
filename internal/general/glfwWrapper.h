#ifndef GLFW_WRAPPER_H_
#define GLFW_WRAPPER_H_

#include "general.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

class glfwWrapper {
private:
    const char* glslVersion;
    GLFWwindow* window = nullptr;
    float mainScale;
    ImVec4 clearColor;

public:
    void createGLFWContext() {
        // GL 3.0 + GLSL 130
        glslVersion = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

        // Create window with graphics context
        mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
        window = glfwCreateWindow((int)(1280 * mainScale), (int)(800 * mainScale), "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
        if (window == nullptr)
            return;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }

    void renderGLFW() {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    const char* getGlslVersion() { return glslVersion; }
    float getMainScale() { return mainScale; }
    GLFWwindow* getWindow() { return window; }

    ~glfwWrapper() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

#endif // !GLFW_WRAPPER_H_
