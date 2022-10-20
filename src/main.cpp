#include <cstdlib>

#include "window.hpp"
#include "render.hpp"
#include "player.hpp"

int main() {
    // setup
    Settings settings = Settings();
    Player player = Player();
    Window window = Window(settings, player); // create glfw window and configure callback functions
    Render render = Render(); // load openGL with glad and configure openGL


    // game loop
    // ---- ---- ----
    while (!window.shouldClose()) {
        // process input
        // ---- ---- ----
        window.processInput(player);


        // update
        // ---- ---- ----


        // render
        // ---- ---- ----
        render.render();

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        window.swapBuffers();
        window.pollEvents();
    }
    
    return EXIT_SUCCESS;
}