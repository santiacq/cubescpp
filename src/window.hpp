#include "../lib/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "player.hpp"

class Window {
    private:
        GLFWwindow* windowPtr;
    public:
        Window();
        ~Window();
        bool shouldClose();
        void swapBuffers();
        void pollEvents();
        void processInput(Player &player);
};