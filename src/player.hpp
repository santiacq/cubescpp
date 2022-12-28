#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "block.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

#define PLAYER_SPEED (float) 2.5
// the range within which the player can break/place blocks
#define PLAYER_BLOCK_RANGE 6

class Player {
    private:
        glm::vec3 pos; // player position
        float pitch, yaw; // angles corresponding to the view direction
        glm::vec3 view; // view direction
        float speed;
        unsigned int currentBlockIndex; //index of the block selected to place next in the placeableBlocks array
    public:
        Player();
        void moveFront(float moveDistance);
        void moveBack(float moveDistance);
        void moveLeft(float moveDistance);
        void moveRight(float moveDistance);
        void moveUp(float moveDistance);
        void moveDown(float moveDistance);
        
        glm::vec3 getPos();
        glm::vec3 getView();
        void setPitch(float value);
        float getPitch();
        void setYaw(float value);
        float getYaw();
        void updateView(); // update view vector based on pitch and yaw
        int getChunkX();
        int getChunkZ();
        float getSpeed();
        void increaseCurrentBlock();
        void decreaseCurrentBlock();
        unsigned int getCurrentBlockIndex();
};

#endif