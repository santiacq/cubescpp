#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp>

class Player {
    private:
        glm::vec3 pos; // player position
        float pitch, yaw; // angles corresponding to the view direction
        glm::vec3 view; // view direction
        float speed;
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

        // used for debugging
        glm::vec3 lastraypos;
        glm::vec3 lastrayview;
};

#endif