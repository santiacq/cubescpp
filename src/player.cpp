#include "player.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#define SPEED (float) 0.5
#define UP glm::vec3(0,1,0)

Player::Player() {
    this->pos = glm::vec3(0, 10, 0);
    this->view = glm::vec3(0.017182, -0.999848, 0);
    this->pitch = -89;
    this->yaw = 100.1;
}
void Player::moveFront(){
    this->pos -= glm::normalize(glm::cross(glm::cross(this->view, UP), UP)) * SPEED;
}
void Player::moveBack(){
    this->pos += glm::normalize(glm::cross(glm::cross(this->view, UP), UP)) * SPEED;
}
void Player::moveLeft(){
    this->pos -= glm::normalize(glm::cross(this->view, UP)) * SPEED;
}
void Player::moveRight(){
    this->pos += glm::normalize(glm::cross(this->view, UP)) * SPEED;
}
void Player::moveUp(){
    this->pos += UP * SPEED;
}
void Player::moveDown(){
    this->pos -= UP * SPEED;
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