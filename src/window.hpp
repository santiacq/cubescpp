#include "../lib/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "player.hpp"
#include "settings.hpp"
#include "clock.hpp"
#include "world.hpp"

class Window {
    private:
        GLFWwindow* windowPtr;
        // attributes used in callback functions
        Settings* settings;
        Player* player;
        World* world;
        float lastX;
        float lastY;
        bool firstMouse;
    public:
        Window(Settings &settings, Player &player, World &world);
        ~Window();
        bool shouldClose();
        void swapBuffers();
        void pollEvents();
        void processInput(Player &player, Clock clock);
        // setters and getters for the attributes used in callback functions
        Settings* getSettings();
        Player* getPlayer();
        void setLastX(float value);
        float getLastX();
        void setLastY(float value);
        float getLastY();
        void setFirstMouse(bool value);
        bool getFirstMouse();
};