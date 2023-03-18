#include "block.hpp"
Block::Block() {}
Block::Block(Blocktype type) {
    this->type = type;
}
Blocktype Block::getType() {
    return this->type;
}

bool Block::isTransparent() {
    for (Blocktype bt : transparentBlocks) {
        if (this->getType() == bt)
            return true;
    }
    return false;
}