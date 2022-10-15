#include "window.hpp"
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Window::Window() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
   windowPtr = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (windowPtr == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(windowPtr);
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
}

Window::~Window() {
    glfwTerminate();
}

void Window::swapBuffers() {
    glfwSwapBuffers(windowPtr);
}

void Window::pollEvents() {
    glfwPollEvents();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(windowPtr);
}

void Window::processInput(Player &player) {
    if (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(windowPtr, true);

    //float cameraSpeed = (float)(state.playerSpeed * state.delta);
    if (glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS)
        player.moveFront();
    if (glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS)
        player.moveBack();
    if (glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS)
        player.moveLeft();
    if (glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS)
        player.moveRight();
    if (glfwGetKey(windowPtr, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.moveUp();
    if (glfwGetKey(windowPtr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        player.moveDown();
}