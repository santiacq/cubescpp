#include <cstdlib>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "window.hpp"
#include "render.hpp"
#include "player.hpp"

int main() {
    // setup
    Settings settings = Settings();
    Player player = Player();
    Window window = Window(settings, player); // create glfw window and configure callback functions
    Render render = Render(settings); // load openGL with glad and configure openGL

    World world = World();

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
        render.render(player, world);

        // debug
        std::cout << "Pos: " << glm::to_string(player.getPos()) << " ; View: " <<
        glm::to_string(player.getView()) << " ; Pitch: "
        <<  player.getPitch() << " ; Yaw: "
        << player.getYaw() << std::endl;

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        window.swapBuffers();
        window.pollEvents();
    }
    
    return EXIT_SUCCESS;
}