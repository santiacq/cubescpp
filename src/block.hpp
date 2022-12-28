#ifndef BLOCK_HPP
#define BLOCK_HPP

enum Blocktype {
    Air,
    Grass,
    Dirt,
    Stone,
    Water,
    Wood
};
typedef enum Blocktype Blocktype;

const Blocktype placeableBlocks[] {Grass, Dirt, Stone, Wood};

class Block {
    private:
        Blocktype type;
    public:
        Block();
        Block(Blocktype type);
        Blocktype getType();
};

#endif