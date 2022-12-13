#include "player.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include "chunk.hpp" // chunk size is needed to calculate current chunk

#define SPEED (float) 2.5
#define UP glm::vec3(0,1,0)

Player::Player() {
    this->pos = glm::vec3(0, 20, 0);
    this->view = glm::vec3(0.017182, -0.999848, 0);
    this->pitch = -89;
    this->yaw = 100.1;
    
    this->speed = SPEED;
}
void Player::moveFront(float moveDistance){
    this->pos -= glm::normalize(glm::cross(glm::cross(this->view, UP), UP)) * moveDistance;
}
void Player::moveBack(float moveDistance){
    this->pos += glm::normalize(glm::cross(glm::cross(this->view, UP), UP)) * moveDistance;
}
void Player::moveLeft(float moveDistance){
    this->pos -= glm::normalize(glm::cross(this->view, UP)) * moveDistance;
}
void Player::moveRight(float moveDistance){
    this->pos += glm::normalize(glm::cross(this->view, UP)) * moveDistance;
}
void Player::moveUp(float moveDistance){
    this->pos += UP * moveDistance;
}
void Player::moveDown(float moveDistance){
    this->pos -= UP * moveDistance;
}

glm::vec3 Player::getPos() {
    return pos;
}
glm::vec3 Player::getView() {
    return view;
}
void Player::setPitch(float value) {
    pitch = value;
}
float Player::getPitch() {
    return pitch;
}
void Player::setYaw(float value) {
    yaw = value;
}
float Player::getYaw() {
    return yaw;
}
void Player::updateView() { // update view vector based on pitch and yaw
    view.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    view.y = glm::sin(glm::radians(pitch));
    view.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    view = glm::normalize(view);
}

int Player::getChunkX() {
    if (pos.x >= -0.5) {
        return (pos.x + 0.5) / 16;
    } else {
        return ((pos.x + 0.5) / 16) - 1;
    }
}
int Player::getChunkZ() {
    if (pos.z >= -0.5) {
        return (pos.z + 0.5) / 16;
    } else {
        return ((pos.z + 0.5) / 16) - 1;
    }
}

float Player::getSpeed() {
    return speed;
}