#include "window.hpp"
#include <iostream>

static void framebuffer_size_callback(GLFWwindow* windowPtr, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static void keyboard_callback(GLFWwindow* windowPtr, int key, int scancode, int action, int mods) {
    Window* window = (Window*) glfwGetWindowUserPointer(windowPtr);
    if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        window->getSettings()->setWireframe(!window->getSettings()->getWireframe());
        if (window->getSettings()->getWireframe()) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

static void mouse_callback(GLFWwindow* windowPtr, double xposIn, double yposIn) {
    Window* window = (Window*) glfwGetWindowUserPointer(windowPtr);

    float xpos = (float) xposIn;
    float ypos = (float) yposIn;

    if (window->getFirstMouse())
    {
        window->setLastX(xpos);
        window->setLastY(ypos);
        window->setFirstMouse(false);
    }

    float xoffset = xpos - window->getLastX();
    float yoffset = window->getLastY() - ypos; // reversed since y-coordinates go from bottom to top
    window->setLastX(xpos);
    window->setLastY(ypos);

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    window->getPlayer()->setYaw(window->getPlayer()->getYaw() + xoffset);
    window->getPlayer()->setPitch(window->getPlayer()->getPitch() + yoffset);

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (window->getPlayer()->getPitch() > 89.0f)
        window->getPlayer()->setPitch(89.0f);
    if (window->getPlayer()->getPitch() < -89.0f)
        window->getPlayer()->setPitch(-89.0f);

    window->getPlayer()->updateView();
}

Window::Window(Settings &settings, Player &player) {
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
   windowPtr = glfwCreateWindow(settings.getScreenWidth(), settings.getScreenHeight(), "cubescpp", NULL, NULL);
    if (windowPtr == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(windowPtr);
    // capture mouse
    glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // set initial attributes
    this->settings = &settings;
    this->player = &player;
    firstMouse = true;

    // set glfw user pointer
    glfwSetWindowUserPointer(windowPtr,this); 

    // set callback functions
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
    glfwSetCursorPosCallback(windowPtr, mouse_callback);
    glfwSetKeyCallback(windowPtr, keyboard_callback);
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

// setters and getters for the attributes used in callback functions
Settings* Window::getSettings() {
    return settings;
}
Player* Window::getPlayer() {
    return player;
}
void Window::setLastX(float value) {
    lastX = value;
}
float Window::getLastX() {
    return lastX;
}
void Window::setLastY(float value) {
    lastY = value;
}
float Window::getLastY() {
    return lastY;
}
void Window::setFirstMouse(bool value) {
    firstMouse = value;
}
bool Window::getFirstMouse() {
    return firstMouse;
}