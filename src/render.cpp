#include "render.hpp"
#include "../lib/glad/include/glad/glad.h"

void Render::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}