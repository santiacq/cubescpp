#include "../lib/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "render.hpp"

Render::Render() {
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // configure z-buffer for opengl global state
    glEnable(GL_DEPTH_TEST);
    // enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Render::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}