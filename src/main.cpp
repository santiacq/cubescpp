#include <cstdlib>

#include "window.hpp"
#include "render.hpp"

int main() {
    // setup
    Window window = Window();
    Render render = Render();

    // render loop
    // ----
    while (!window.shouldClose()) {
        // process input
        // ----
        window.processInput();


        // update
        // ----


        // render
        // ----
        render.render();

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        window.swapBuffers();
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}