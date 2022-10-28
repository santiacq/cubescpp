#include "player.hpp"
#include "shader.hpp"
#include "settings.hpp"
#include "world.hpp"
#include "mesh.hpp"
#include "atlas.hpp"

class Render {
    private:
        unsigned int VBO, VAO;
        Shader shader;
        Atlas atlas;
    public:
        Render(Settings settings);
        void render(Player player, World &world);
        void updateProjectionMatrix(Settings settings);
};