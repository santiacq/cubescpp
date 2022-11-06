#include "clock.hpp"
#include <GLFW/glfw3.h>

Clock::Clock() {
    delta = 0;
    lastFrame = 0;
}

void Clock::tick() {
    float currentFrame = (float) glfwGetTime();
    delta = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

float Clock::getDelta() {
    return this->delta;
}