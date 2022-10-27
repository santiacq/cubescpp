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
    unsigned int atlasPosition;
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
    
    // configure opengl to draw the texture correctly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // GL_RGBA may need to be replaced with GL_RGB in certain textures
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}