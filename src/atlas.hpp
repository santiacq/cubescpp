#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "block.hpp"

enum Ubication {
    Top,
    Side,
    Bottom
};
typedef enum Ubication Ubication;

class Atlas {
    private:
        unsigned int size;
        unsigned int atlasPosition(Block b, Ubication u);
    public:
        Atlas();
        Atlas(const char* texturePath, unsigned int size);
        float getU(Block b, Ubication u);
        float getV(Block b, Ubication u);
        float getD();
};

#endif