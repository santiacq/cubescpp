#include "atlas.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#include "../lib/glad/include/glad/glad.h"

#define TEXTURE_GRASS_TOP 0
#define TEXTURE_GRASS_SIDE 1
#define TEXTURE_GRASS_BOTTOM 2
#define TEXTURE_DIRT_TOP 2
#define TEXTURE_DIRT_SIDE 2
#define TEXTURE_DIRT_BOTTOM 2
#define TEXTURE_STONE_TOP 3
#define TEXTURE_STONE_SIDE 3
#define TEXTURE_STONE_BOTTOM 3
#define TEXTURE_WATER_TOP 4
#define TEXTURE_WATER_SIDE 4
#define TEXTURE_WATER_BOTTOM 4
#define TEXTURE_WOOD_TOP 5
#define TEXTURE_WOOD_SIDE 5
#define TEXTURE_WOOD_BOTTOM 5
#define TEXTURE_LOG_TOP 14
#define TEXTURE_LOG_SIDE 6
#define TEXTURE_LOG_BOTTOM 14
#define TEXTURE_LEAVES_TOP 7
#define TEXTURE_LEAVES_SIDE 7
#define TEXTURE_LEAVES_BOTTOM 7
#define TEXTURE_ROCKS_TOP 11
#define TEXTURE_ROCKS_SIDE 11
#define TEXTURE_ROCKS_BOTTOM 11
#define TEXTURE_SAND_TOP 13
#define TEXTURE_SAND_SIDE 13
#define TEXTURE_SAND_BOTTOM 13

// macro for the case inside the switch statement for each blocktype
#define SWITCH(BLOCK) {\
    switch (u) {\
        case Top:\
            atlasPosition = TEXTURE_##BLOCK##_TOP;\
            break;\
        case Side:\
            atlasPosition = TEXTURE_##BLOCK##_SIDE;\
            break;\
        case Bottom:\
            atlasPosition = TEXTURE_##BLOCK##_BOTTOM;\
            break;\
    }\
}

unsigned int Atlas::atlasPosition(Block b, Ubication u) {
    unsigned int atlasPosition = 0;
    switch (b.getType()) {
        case Grass:
            SWITCH(GRASS);
            break;
        case Dirt:
            SWITCH(DIRT);
            break;
        case Stone:
            SWITCH(STONE);
            break;
        case Water:
            SWITCH(WATER);
            break;
        case Wood:
            SWITCH(WOOD);
            break;
        case Log:
            SWITCH(LOG);
            break;
        case Leaves:
            SWITCH(LEAVES);
            break;
        case Rocks:
            SWITCH(ROCKS);
            break;
        case Sand:
            SWITCH(SAND);
            break;
        default:
            return 0; // this won't happen unless blocktype is passed wrong
    }
    return atlasPosition;
}

float Atlas::getU(Block b, Ubication u){
    unsigned int atlasPosition = this->atlasPosition(b, u); // the position this texture has on the atlas
    return trunc(fmod(atlasPosition, this->size)) * 1.0f / (float) this->size;
}
float Atlas::getV(Block b, Ubication u){
    unsigned int atlasPosition = this->atlasPosition(b, u); // the position this texture has on the atlas
    return trunc((float)atlasPosition / (float)this->size) * 1.0f / (float)this->size;
}
float Atlas::getD() {
    return 1.0f / (float) this->size;
}

Atlas::Atlas() {}

Atlas::Atlas(const char* texturePath, unsigned int size){
    // set atlas size
    this->size = size;
    // load image
    stbi_set_flip_vertically_on_load(1);  
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0); 
    // configure opengl to use this texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // GL_RGBA may need to be replaced with GL_RGB in certain textures
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // generate mipmap
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    // configure opengl to draw the texture correctly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    
    stbi_image_free(data);
}