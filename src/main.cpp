#include <cstdlib>

#include "window.hpp"
#include "render.hpp"
#include "player.hpp"

int main() {
    // setup
    Window window = Window();
    Render render = Render();
    Player player = Player();

    // render loop
    // ----
    while (!window.shouldClose()) {
        // process input
        // ----
        window.processInput(player);


        // update
        // ----


        // render
        // ----
        render.render();

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        window.swapBuffers();
        window.pollEvents();
    }
    
    return EXIT_SUCCESS;
}