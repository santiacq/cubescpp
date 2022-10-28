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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // set up vertex buffer object and vertex array object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // build and compile shader program
    shader = Shader("shaders/testvert.glsl", "shaders/testfrag.glsl");
    
    // set projection matrix 
    updateProjectionMatrix(settings);
    
    // load texture atlas
    atlas = Atlas("textures/atlas.png", 8);
}

void Render::updateProjectionMatrix(Settings settings) {
    shader.use();
    glm::mat4 projection = glm::perspective(settings.getFov(), (float) settings.getScreenWidth() / (float) settings.getScreenHeight(), settings.getNearViewDistance(), settings.getFarViewDistance());
    shader.setFloatMatrix4("projection", (float*) &projection);
}

void Render::render(Player player, World &world) {
    // Clear color buffer and z buffer
    glClearColor(0.1f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Update view matrix
    shader.use();
    glm::mat4 view = glm::lookAt(player.getPos(), player.getPos() + player.getView(), UP);
    shader.setFloatMatrix4("view", (float*) &view);
    
    // Render world

    // update chunk mesh, this should only be done if necesary (chunk updated)
    Chunk* chunk = world.getChunk(player.getChunkX(), player.getChunkZ());
    chunk->updateMesh(this->atlas);

    // update model matrix (which moves each chunk)
    glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(player.getChunkX()*16,0,player.getChunkZ()*16));
    shader.setFloatMatrix4("model", (float*) &model);

    // tell opengl the format of the vertex attributes being passed
    glBindVertexArray(VAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // pass the data to opengl
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, chunk->getMesh()->getTriangles() * FLOATS_PER_TRIANGLE * sizeof(float), chunk->getMesh()->getVertices(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, chunk->getMesh()->getTriangles() * FLOATS_PER_TRIANGLE / 3);
}