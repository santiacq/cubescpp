#include "../lib/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <iostream>

#include "render.hpp"
#include "shader.hpp"

#define UP glm::vec3(0,1,0)

Render::Render(Settings settings) {
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
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // set up vertex buffer object and vertex array object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // build and compile shader program
    shader = Shader("shaders/testvert.glsl", "shaders/testfrag.glsl");
    updateProjectionMatrix(settings);
    // set projection matrix 


    // load texture atlas

}

void Render::updateProjectionMatrix(Settings settings) {
    shader.use();
    glm::mat4 projection = glm::perspective(settings.getFov(), (float) settings.getScreenWidth() / (float) settings.getScreenHeight(), settings.getNearViewDistance(), settings.getFarViewDistance());
    shader.setFloatMatrix4("projection", (float*) &projection);
}

void Render::render(Player player) {
    // Clear color buffer and z buffer
    glClearColor(0.1f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Update view matrix
    shader.use();
    glm::mat4 view = glm::lookAt(player.getPos(), player.getPos() + player.getView(), UP);
    shader.setFloatMatrix4("view", (float*) &view);
    // Render world
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        0.0f,  0.0f, 100.0f,
        100.0f, 0.0f, 0.0f
    };
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    shader.use();
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
}