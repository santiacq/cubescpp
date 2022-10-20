#include "settings.hpp"

Settings::Settings() {
    wireframe = false;
}

void Settings::setWireframe(bool value) {
    wireframe = value;
}

bool Settings::getWireframe() {
    return wireframe;
}