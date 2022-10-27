#include "block.hpp"
Block::Block() {}
Block::Block(Blocktype type) {
    this->type = type;
}
Blocktype Block::getType() {
    return this->type;
}