#include "player.hpp"
#include "shader.hpp"
#include "settings.hpp"
#include "world.hpp"
#include "mesh.hpp"
#include "atlas.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

class Render {
    private:
        unsigned int VBO, VAO;
        Shader chunkShader;
        Shader GUIShader;
        Atlas atlas;
        glm::mat4 projection, view;
        std::vector<Chunk*> getVisibleChunks(Player player, World &world, Settings settings);
    public:
        Render(Settings settings);
        void render(Player player, World &world, Settings settings);
        void updateProjectionMatrix(Settings settings);
};