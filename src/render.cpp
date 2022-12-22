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
    chunkShader = Shader("shaders/chunkvert.glsl", "shaders/chunkfrag.glsl");
    GUIShader = Shader("shaders/guivert.glsl", "shaders/guifrag.glsl");
    
    // set projection matrix 
    updateProjectionMatrix(settings);
    
    // load texture atlas
    atlas = Atlas("textures/atlas.png", 8);
}

void Render::updateProjectionMatrix(Settings settings) {
    chunkShader.use();
    this->projection = glm::perspective(settings.getFov(), (float) settings.getScreenWidth() / (float) settings.getScreenHeight(), settings.getNearViewDistance(), settings.getFarViewDistance());
    chunkShader.setFloatMatrix4("projection", (float*) &projection);
}

// frustum culling / flood
std::vector<Chunk*> Render::getVisibleChunks(Player player, World &world, Settings settings) {
    std::vector<Chunk*> visibleChunks;
    /*visibleChunks.push_back(world.getChunk(player.getChunkX(), player.getChunkZ()));
    visibleChunks.push_back(world.getChunk(player.getChunkX() + 1, player.getChunkZ()));
    visibleChunks.push_back(world.getChunk(player.getChunkX(), player.getChunkZ() + 1));
    visibleChunks.push_back(world.getChunk(player.getChunkX() - 1, player.getChunkZ()));
    visibleChunks.push_back(world.getChunk(player.getChunkX(), player.getChunkZ() - 1));
    visibleChunks.push_back(world.getChunk(player.getChunkX() + 1, player.getChunkZ() + 1));
    visibleChunks.push_back(world.getChunk(player.getChunkX() - 1, player.getChunkZ() + 1));
    visibleChunks.push_back(world.getChunk(player.getChunkX() + 1, player.getChunkZ() - 1));
    visibleChunks.push_back(world.getChunk(player.getChunkX() - 1, player.getChunkZ() - 1));*/
    for (int x = -3; x <= 3; x++) {
        for (int z = -3; z <= 3; z++) {
            visibleChunks.push_back(world.getChunk(player.getChunkX() + x, player.getChunkZ() + z));
        }
    }
    return visibleChunks;
}

void Render::render(Player player, World &world, Settings settings) {
    // Clear color buffer and z buffer
    glClearColor(0.1f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Update view matrix
    chunkShader.use();
    this->view = glm::lookAt(player.getPos(), player.getPos() + player.getView(), UP);
    chunkShader.setFloatMatrix4("view", (float*) &view);
    
    // Render world

    // choose which chunks to render using frustum culling and put them in a vector
    std::vector<Chunk*> visibleChunks = getVisibleChunks(player, world, settings);

    for (int i = 0; i < (int) visibleChunks.size(); i++) {
        // update chunk mesh, this should only be done if necesary (chunk updated)
        if (visibleChunks[i]->getMesh() == NULL || visibleChunks[i]->isMeshOutdated) {
            visibleChunks[i]->updateMesh(this->atlas, world);
        }

        // update model matrix (which moves each chunk)
        glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(visibleChunks[i]->getChunkX()*16,0,visibleChunks[i]->getChunkZ()*16));
        chunkShader.setFloatMatrix4("model", (float*) &model);

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
        glBufferData(GL_ARRAY_BUFFER, visibleChunks[i]->getMesh()->getTriangles() * FLOATS_PER_TRIANGLE * sizeof(float), visibleChunks[i]->getMesh()->getVertices(), GL_DYNAMIC_DRAW);
        // draw the blocks
        glDrawArrays(GL_TRIANGLES, 0, visibleChunks[i]->getMesh()->getTriangles() * FLOATS_PER_TRIANGLE / 3);
    }
    
    // Render GUI
    GUIShader.use();
    glDisable(GL_DEPTH_TEST);

    float vertices[] = {
        // vertical
        -0.001f , 0.015f    , 0.0f,
        -0.001f , -0.015f   , 0.0f,
        0.001f  , -0.015f   , 0.0f,
        0.001f  , -0.015f   , 0.0f,
        0.001f  , 0.015f    , 0.0f,
        -0.001f , 0.015f    , 0.0f,
        // horizontal
        -0.008f , 0.002f    , 0.0f,
        -0.008f , -0.002f   , 0.0f,
        0.008f  , -0.002f   , 0.0f,
        0.008f  , -0.002f   , 0.0f,
        0.008f  , 0.002f    , 0.0f,
        -0.008f , 0.002f    , 0.0f,
    }; 

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 12);
    
    glEnable(GL_DEPTH_TEST);
}