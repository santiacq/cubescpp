#include <cstdlib>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "window.hpp"
#include "render.hpp"
#include "player.hpp"
#include "clock.hpp"

int main() {
    // setup
    Settings settings = Settings();
    Player player = Player();
    World world = World();
    Window window = Window(settings, player, world); // create glfw window and configure callback functions
    Render render = Render(settings); // load openGL with glad and configure openGL
    Clock clock = Clock();
    

    // game loop
    // ---- ---- ----
    while (!window.shouldClose()) {
        // calculate delta
        // ---- ---- ----
        clock.tick();
        //std::cout << "FPS: " << 1/clock.getDelta() << std::endl;

        // process input
        // ---- ---- ----
        window.processInput(player, clock);

        // update
        // ---- ---- ----

        // render
        // ---- ---- ----
        render.render(player, world, settings);

        // debug
        /*std::cout << "Pos: " << glm::to_string(player.getPos()) << " ; View: " <<
        glm::to_string(player.getView()) << " ; Pitch: " <<  player.getPitch() << " ; Yaw: "
        << player.getYaw() << " ; chunkX: " << player.getChunkX()  << " ; chunkZ: " << player.getChunkZ() << std::endl;
        */

        // swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        window.swapBuffers();
        window.pollEvents();
    }
    
    return EXIT_SUCCESS;
}