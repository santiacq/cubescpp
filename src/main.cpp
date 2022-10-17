#include <cstdlib>

#include "window.hpp"
#include "render.hpp"
#include "player.hpp"

int main() {
    // setup
    Window window = Window(); // create glfw window and configure callback functions
    Render render = Render(); // load openGL with glad and configure openGL
    Player player = Player();

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