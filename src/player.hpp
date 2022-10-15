#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/vec3.hpp>

class Player {
    private:
        glm::vec3 pos; // player position
        glm::vec3 view; // view direction
    public:
        Player();
        void moveFront();
        void moveBack();
        void moveLeft();
        void moveRight();
        void moveUp();
        void moveDown();
};

#endif