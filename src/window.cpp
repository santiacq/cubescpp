#include "window.hpp"
#include "chunk.hpp"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
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

    window->getPlayer()->setYaw(fmod((window->getPlayer()->getYaw() + xoffset), 360));
    window->getPlayer()->setPitch(window->getPlayer()->getPitch() + yoffset);

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (window->getPlayer()->getPitch() > 89.0f)
        window->getPlayer()->setPitch(89.0f);
    if (window->getPlayer()->getPitch() < -89.0f)
        window->getPlayer()->setPitch(-89.0f);

    window->getPlayer()->updateView();
}

static int sign(float x) { // returns 1 if x is positive, -1 if negative, 0 if 0.
    return (x > 0) - (x < 0);
}

static float calculateTMax(float rayOrigin, float rayDirection) {
    if (rayDirection < 0) {
        return calculateTMax(-rayOrigin, -rayDirection);
    } else {
        rayOrigin = rayOrigin - (int) rayOrigin;
        return (1 - rayOrigin) / rayDirection;
    }
}

static void mouse_button_callback(GLFWwindow* windowPtr, int button, int action, int mods) {
    Window* window = (Window*) glfwGetWindowUserPointer(windowPtr);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {      
        // break a block
        /* first we calculate the coordinates of the block that will be broken. we call these updatedX, updatedY and updatedZ.
          this is done using ray casting. the algorithm is based on the paper "A Fast Voxel Traversal Algorithm for Ray Tracing" 
          from John Amanatides and Andrew Woo
        */
        Player* player = window->getPlayer();
        // calculate initial variables
        unsigned int iterX = (int) (round(player->getPos().x) - (player->getChunkX()*CHUNK_SIZE));
        unsigned int iterY = (int) player->getPos().y;
        unsigned int iterZ = (int) (round(player->getPos().z) - (player->getChunkZ()*CHUNK_SIZE));

        int stepX = sign(player->getView().x);
        int stepY = sign(player->getView().y);
        int stepZ = sign(player->getView().z);

        float tMaxX = calculateTMax(player->getPos().x, player->getView().x);
        float tMaxY = calculateTMax(player->getPos().y, player->getView().y);
        float tMaxZ = calculateTMax(player->getPos().z, player->getView().z);

        float tDeltaX = stepX / player->getView().x;
        float tDeltaY = stepY / player->getView().y;
        float tDeltaZ = stepZ / player->getView().z;

        bool found = false;
        float distanceLimit = 30;
        Chunk* updatedChunk = window->getWorld()->getChunk(player->getChunkX(), player->getChunkZ());

        while (!found && (tMaxX < distanceLimit || tMaxY < distanceLimit || tMaxZ < distanceLimit)) {
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    iterX += stepX;
                    tMaxX += tDeltaX;
                } else {
                    iterZ += stepZ;
                    tMaxZ += tDeltaZ;
                }
            } else {
                if (tMaxY < tMaxZ) {
                    iterY += stepY;
                    tMaxY += tDeltaY;
                } else {
                    iterZ += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            if (updatedChunk->getBlock(iterX, iterY, iterZ).getType() != Air) {
                found = true;
            } 
        }

        if (found) {
            updatedChunk->updateBlock(iterX, iterY, iterZ, Air);
        }

        /*unsigned int updatedX = (int) (round(player->getPos().x) - (player->getChunkX()*CHUNK_SIZE));
        unsigned int updatedY = (int) player->getPos().y - 2;
        unsigned int updatedZ = (int) (round(player->getPos().z) - (player->getChunkZ()*CHUNK_SIZE)); */

        //Chunk* updatedChunk = window->getWorld()->getChunk(player->getChunkX(), player->getChunkZ());
        //updatedChunk->updateBlock(updatedX, updatedY, updatedZ, Air);
    }
}

Window::Window(Settings &settings, Player &player, World &world) {
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
    
    // remove fps cap, comment this line to set 60 fps cap
    glfwSwapInterval(0);
    // capture mouse
    glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // set initial attributes
    this->settings = &settings;
    this->player = &player;
    this->world = &world;
    firstMouse = true;

    // set glfw user pointer
    glfwSetWindowUserPointer(windowPtr,this); 

    // set callback functions
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
    glfwSetCursorPosCallback(windowPtr, mouse_callback);
    glfwSetKeyCallback(windowPtr, keyboard_callback);
    glfwSetMouseButtonCallback(windowPtr, mouse_button_callback);
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

void Window::processInput(Player &player, Clock clock) {
    if (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(windowPtr, true);

    float moveDistance = player.getSpeed() * clock.getDelta(); // the distance player will move in this frame
    if (glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS)
        player.moveFront(moveDistance);
    if (glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS)
        player.moveBack(moveDistance);
    if (glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS)
        player.moveLeft(moveDistance);
    if (glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS)
        player.moveRight(moveDistance);
    if (glfwGetKey(windowPtr, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.moveUp(moveDistance);
    if (glfwGetKey(windowPtr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        player.moveDown(moveDistance);
}

// setters and getters for the attributes used in callback functions
Settings* Window::getSettings() {
    return settings;
}
Player* Window::getPlayer() {
    return player;
}
World* Window::getWorld() {
    return world;
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