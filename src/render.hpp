#include "player.hpp"
#include "shader.hpp"
#include "settings.hpp"
#include "chunk.hpp"
#include "mesh.hpp"

class Render {
    private:
        unsigned int VBO, VAO;
        Shader shader;
    public:
        Render(Settings settings);
        void render(Player player, Chunk &chunk);
        void updateProjectionMatrix(Settings settings);
};