#include "window.hpp"
#include "chunk.hpp"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <tuple>
#include <utility>

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

/*  Auxiliary function used in raycast and to calculate placed blocks
    takes the currentChunk, its chunkX and chunkZ coordinates and the coords of the block to be updated
    if the coordinates of the block are outside of the currentChunk (x < 0 || x > CHUNK_SIZE - 1 || z < 0 || z > CHUNK_SIZE - 1)
    it updates the block coordinates, the chunkCoordinates, and returns a pointer to the new currentChunk
    if there's nothing to update it returns its input unmodified
*/
static std::tuple<glm::ivec3, int, int, Chunk*> updateCurrentChunk(glm::vec3 coords, int currentChunkX, int currentChunkZ, Chunk* currentChunk, World* world) {
    // update currentChunk
    int newChunkX = currentChunkX;
    int newChunkZ = currentChunkZ;
    if (coords.x < 0) {
        coords.x = CHUNK_SIZE + coords.x;
        newChunkX = currentChunkX - 1;
    } else if (coords.x > CHUNK_SIZE - 1) {
        coords.x = coords.x - CHUNK_SIZE;
        newChunkX = currentChunkX + 1;
    }
    if (coords.z < 0) {
        coords.z = CHUNK_SIZE + coords.z;
        newChunkZ = currentChunkZ - 1;
    } else if (coords.z > CHUNK_SIZE - 1) {
        coords.z = coords.z - CHUNK_SIZE;
        newChunkZ = currentChunkZ + 1;
    }
    if (newChunkX != currentChunkX || newChunkZ != currentChunkZ) {
        currentChunkX = newChunkX;
        currentChunkZ = newChunkZ;
        currentChunk = world->getChunk(currentChunkX, currentChunkZ);
    }
    return std::tuple<glm::ivec3, int, int, Chunk*>(coords, currentChunkX, currentChunkZ, currentChunk);
}

/*  Raycasting algorithm based on the paper "A Fast Voxel Traversal Algorithm for Ray Tracing" from John Amanatides and Andrew Woo

    Casts a ray from the player position towards the player view vector, and returns a tuple with information 
    about whether it intersected a block in the PLAYER_BLOCK_RANGE, and in case it did it returns the coordinates of
    that block, and the face in which it was intersected
    
    The tuple returned has the following entries:
    0. a boolean that is true if and only if the ray intersected a block
    (the following are irrelevant if 1 == false)
    1. a vector with the coordinates of the intersected block
    2. the chunkX coordinate of the chunk where the intersected block is in
    3. the chunkZ coordinate of the chunk where the intersected block is in
    4. a pointer to the chunk where the intersected block is in
    5. a vector with the normal of the face of the block that ray intersected
*/
static std::tuple<bool, glm::ivec3, int, int, Chunk*, glm::ivec3> raycast(Player* player, World* world) {
    glm::ivec3 coords; // coordinates of current block being checked by the algorithm
    glm::ivec3 normal; // normal of the face of the intersected block
    glm::ivec3 step;
    glm::vec3 origin, direction, tmax, tdelta;
    bool found = false;
    // currentChunk refers to the chunk where the block being currently processed (coords) is in
    int currentChunkX = player->getChunkX();
    int currentChunkZ = player->getChunkZ();
    Chunk* currentChunk = world->getChunk(currentChunkX, currentChunkZ);
    
    coords = {round(player->getPos().x - (player->getChunkX()*CHUNK_SIZE)), floor(player->getPos().y), round(player->getPos().z - (player->getChunkZ()*CHUNK_SIZE))};
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
                coords.x += step.x;
                tmax.x += tdelta.x;
                normal = {-step.x, 0, 0};
            } else {
                coords.z += step.z;
                tmax.z += tdelta.z;
                normal = {0, 0, -step.z};
            }
        } else {
            if (tmax.y < tmax.z) {
                coords.y += step.y;
                tmax.y += tdelta.y;
                normal = {0, -step.y, 0};
            } else {
                coords.z += step.z;
                tmax.z += tdelta.z;
                normal = {0, 0, -step.z};
            }
        }
        if (coords.y < 0 || coords.y > WORLD_HEIGHT - 1) {
            break;
        }

        std::tuple<glm::ivec3, int, int, Chunk*> t = updateCurrentChunk(coords, currentChunkX, currentChunkZ, currentChunk, world);
        coords = std::get<0>(t);
        currentChunkX = std::get<1>(t);
        currentChunkZ = std::get<2>(t);
        currentChunk = std::get<3>(t);

        if (currentChunk->getBlock(coords.x, coords.y, coords.z).getType() != Air) {
            found = true;
        }
    }
    return std::tuple<bool, glm::ivec3, int, int, Chunk*, glm::ivec3>(found, coords, currentChunkX, currentChunkZ, currentChunk, normal);
}

static void mouse_button_callback(GLFWwindow* windowPtr, int button, int action, int mods) {
    Window* window = (Window*) glfwGetWindowUserPointer(windowPtr);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {  
        
        std::tuple<bool, glm::ivec3, int, int, Chunk*, glm::ivec3> t = raycast(window->getPlayer(), window->getWorld());
        bool found = std::get<0>(t);
        glm::ivec3 coords = std::get<1>(t);
        Chunk* currentChunk = std::get<4>(t);

        if (found) {
            currentChunk->updateBlock(coords.x, coords.y, coords.z, Air, window->getWorld());
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        
        std::tuple<bool, glm::ivec3, int, int, Chunk*, glm::ivec3> t = raycast(window->getPlayer(), window->getWorld());
        bool found = std::get<0>(t);
        glm::ivec3 coords = std::get<1>(t);
        int currentChunkX =std::get<2>(t);
        int currentChunkZ =std::get<3>(t);
        Chunk* currentChunk = std::get<4>(t);
        glm::ivec3 normal = std::get<5>(t);

        if (found) {
            std::tuple<glm::ivec3, int, int, Chunk*> t = updateCurrentChunk(coords + normal, currentChunkX, currentChunkZ, currentChunk, window->getWorld());
            coords = std::get<0>(t);
            currentChunkX = std::get<1>(t);
            currentChunkZ = std::get<2>(t);
            currentChunk = std::get<3>(t);
        
            currentChunk->updateBlock(coords.x, coords.y, coords.z, Wood, window->getWorld());
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
   windowPtr = glfwCreateWindow(settings.getScreenWidth(), settings.getScreenHeight(), "cubescpp", glfwGetPrimaryMonitor(), NULL);
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