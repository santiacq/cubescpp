#include "../lib/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <iostream>

#include "block.hpp"
#include "chunk.hpp"
#include "render.hpp"
#include "shader.hpp"

#include "frustum.h"

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
    glm::mat4 m = this->projection * this->view;
    Frustum frustum = Frustum(m);
    
    for (int x = -8; x <= 8; x++) {
        for (int z = -8; z <= 8; z++) {
            Chunk* chunk = world.getChunk(player.getChunkX() + x, player.getChunkZ() + z);
            if (frustum.IsBoxVisible(glm::vec3{-0.5 + chunk->getChunkX()*16, 0, -0.5 + chunk->getChunkZ()*16}, glm::vec3{16.5 + chunk->getChunkX()*16, WORLD_HEIGHT, 16.5 + chunk->getChunkZ()*16})) {
                visibleChunks.push_back(chunk);
            }
        }
    }
    return visibleChunks;
}

void Render::renderWorld(Player player, World &world, Settings settings) {
    // assumes that chunkShader is already being used

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
}

void Render::renderGUI(Player player, Settings settings) {
    GUIShader.use();
    glDisable(GL_DEPTH_TEST);

    float w = settings.getScreenWidth();
    float h = settings.getScreenHeight();
    // crosshair
    GUIShader.setBool("crosshair", true);
    
    float crosshairVertices[] = {
        // vertical                                     // (texture coords)
        -1.0f/(0.4f*w) , 1.0f/(0.07f*h)    , 0.0f,      0.0f, 0.0f,
        -1.0f/(0.4f*w) , -1.0f/(0.07f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.4f*w)  , -1.0f/(0.07f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.4f*w)  , -1.0f/(0.07f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.4f*w)  , 1.0f/(0.07f*h)    , 0.0f,      0.0f, 0.0f,
        -1.0f/(0.4f*w) , 1.0f/(0.07f*h)    , 0.0f,      0.0f, 0.0f,
        // horizontal
        -1.0f/(0.07f*w) , 1.0f/(0.4f*h)    , 0.0f,      0.0f, 0.0f,
        -1.0f/(0.07f*w) , -1.0f/(0.4f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.07f*w)  , -1.0f/(0.4f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.07f*w)  , -1.0f/(0.4f*h)   , 0.0f,      0.0f, 0.0f,
        1.0f/(0.07f*w)  , 1.0f/(0.4f*h)    , 0.0f,      0.0f, 0.0f,
        -1.0f/(0.07f*w) , 1.0f/(0.4f*h)    , 0.0f,      0.0f, 0.0f,
    };

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    // draw currently selected blocktype
    GUIShader.setBool("crosshair", false);

    float m = 200; // variable used to adjust the blocktype square size (the bigger the number the bigger the square)
    Atlas a = this->atlas;
    Blocktype b = placeableBlocks[player.getCurrentBlockIndex()];

    float blocktypeVertices[] = {
        -1.0f           , -1.0f + m*(1/h)   , 0.0f  , a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
        -1.0f           , -1.0f             , 0.0f  , a.getU(b, Side)           ,  a.getV(b, Side)           ,
        -1.0f + m*(1/w) , -1.0f             , 0.0f  , a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
        -1.0f + m*(1/w) , -1.0f             , 0.0f  , a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
        -1.0f + m*(1/w) , -1.0f + m*(1/h)   , 0.0f  , a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
        -1.0f           , -1.0f + m*(1/h)   , 0.0f  , a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
    };

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blocktypeVertices), blocktypeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glEnable(GL_DEPTH_TEST);
}

void Render::render(Player player, World &world, Settings settings) {
    // Clear color buffer and z buffer
    glClearColor(0.4f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Update view matrix
    chunkShader.use();
    this->view = glm::lookAt(player.getPos(), player.getPos() + player.getView(), UP);
    chunkShader.setFloatMatrix4("view", (float*) &view);
    
    // Render world
    this->renderWorld(player, world, settings);
    
    // Render GUI
    this->renderGUI(player, settings);
}