#ifndef BLOCK_HPP
#define BLOCK_HPP

enum Blocktype {
    Air,
    Grass,
    Dirt,
    Stone,
    Water
};
typedef enum Blocktype Blocktype;

class Block {
    private:
        Blocktype type;
    public:
        Block();
        Block(Blocktype type);
        Blocktype getType();
};

#endif