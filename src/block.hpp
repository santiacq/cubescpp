#ifndef BLOCK_HPP
#define BLOCK_HPP

enum Blocktype {
    Air,
    Grass,
    Dirt,
    Stone,
    Water,
    Wood,
    Log,
    Leaves,
    Rocks,
    Sand,
    Glass
};
typedef enum Blocktype Blocktype;

const Blocktype placeableBlocks[] {Grass, Dirt, Stone, Wood, Log, Leaves, Rocks, Sand, Water, Glass};
const Blocktype transparentBlocks[] {Water, Leaves, Glass};

class Block {
    private:
        Blocktype type;
    public:
        Block();
        Block(Blocktype type);
        Blocktype getType();
        bool isTransparent();
};

#endif