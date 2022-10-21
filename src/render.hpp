#include "player.hpp"
#include "shader.hpp"
#include "settings.hpp"

class Render {
    private:
        unsigned int VBO, VAO;
        Shader shader;
    public:
        Render(Settings settings);
        void render(Player player);
        void updateProjectionMatrix(Settings settings);
};