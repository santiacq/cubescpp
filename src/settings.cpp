#include "settings.hpp"

Settings::Settings() {
    // default values
    screenWidth = 1920;
    screenHeight = 1080;
    nearViewDistance = 0.1;
    farViewDistance = 1000;
    fov = 45;
    renderDistance = 8;
    wireframe = false;
}

void Settings::setScreenWidth(unsigned int value) {
    screenWidth = value;
}
unsigned int Settings::getScreenWidth() {
    return screenWidth;
}
void Settings::setScreenHeight(unsigned int value) {
    screenHeight = value;
}
unsigned int Settings::getScreenHeight() {
    return screenHeight;
}
void Settings::setNearViewDistance(float value) {
    nearViewDistance = value;
}
float Settings::getNearViewDistance() {
    return nearViewDistance;
}
void Settings::setFarViewDistance(float value) {
    farViewDistance = value;
}
float Settings::getFarViewDistance() {
    return farViewDistance;
}
void Settings::setFov(float value) {
    fov = value;
}
float Settings::getFov() {
    return fov;
}
void Settings::setRenderDistance(unsigned int value) {
    renderDistance = value;
}
unsigned int Settings::getRenderDistance() {
    return renderDistance;
}
void Settings::setWireframe(bool value) {
    wireframe = value;
}
bool Settings::getWireframe() {
    return wireframe;
}