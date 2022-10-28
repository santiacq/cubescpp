#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/vec3.hpp>

class Player {
    private:
        glm::vec3 pos; // player position
        float pitch, yaw; // angles corresponding to the view direction
        glm::vec3 view; // view direction
    public:
        Player();
        void moveFront();
        void moveBack();
        void moveLeft();
        void moveRight();
        void moveUp();
        void moveDown();
        
        glm::vec3 getPos();
        glm::vec3 getView();
        void setPitch(float value);
        float getPitch();
        void setYaw(float value);
        float getYaw();
        void updateView(); // update view vector based on pitch and yaw
        int getChunkX();
        int getChunkZ();
};

#endif