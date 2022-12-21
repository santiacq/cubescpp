#include "window.hpp"
#include "chunk.hpp"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/fwd.hpp>
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

static void mouse_button_callback(GLFWwindow* windowPtr, int button, int action, int mods) {
    Window* window = (Window*) glfwGetWindowUserPointer(windowPtr);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {  
        // Raycasting algorithm based on the paper "A Fast Voxel Traversal Algorithm for Ray Tracing" from John Amanatides and Andrew Woo
        Player* player = window->getPlayer();

        glm::ivec3 iter; // coordinates of current block being checked by the algorithm
        glm::ivec3 step;
        glm::vec3 origin, direction, tmax, tdelta;
        bool found = false;
        Chunk* updatedChunk = window->getWorld()->getChunk(player->getChunkX(), player->getChunkZ());

        iter = {round(player->getPos().x - (player->getChunkX()*CHUNK_SIZE)), floor(player->getPos().y), round(player->getPos().z - (player->getChunkZ()*CHUNK_SIZE))};
        origin = player->getPos();
        direction = player->getView();
        step = {sign(direction.x), sign(direction.y), sign(direction.z)};

        tmax.x = (round(origin.x) - origin.x + step.x * 0.5) / (direction.x);
        tmax.y = (direction.y > 0 ? (ceilf(origin.y) - origin.y) : (origin.y - floorf(origin.y))) / std::abs(direction.y);
        tmax.z = (round(origin.z) - origin.z + step.z * 0.5) / (direction.z);

        tdelta = {(float)step.x / direction.x, (float)step.y / direction.y, (float)step.z / direction.z};

        while (!found && (tmax.x < PLAYER_BLOCK_RANGE || tmax.y < PLAYER_BLOCK_RANGE || tmax.z < PLAYER_BLOCK_RANGE)) {
            if (tmax.x < tmax.y) {
                if (tmax.x < tmax.z) {
                    iter.x += step.x;
                    tmax.x += tdelta.x;
                } else {
                    iter.z += step.z;
                    tmax.z += tdelta.z;
                }
            } else {
                if (tmax.y < tmax.z) {
                    iter.y += step.y;
                    tmax.y += tdelta.y;
                } else {
                    iter.z += step.z;
                    tmax.z += tdelta.z;
                }
            }
            if (updatedChunk->getBlock(iter.x, iter.y, iter.z).getType() != Air) {
                found = true;
            }
        }
        if (found) {
            updatedChunk->updateBlock(iter.x, iter.y, iter.z, Air);
        }
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