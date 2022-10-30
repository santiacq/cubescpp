#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "block.hpp"
#include "mesh.hpp"
#include "atlas.hpp"

#define WORLD_HEIGHT 128
#define CHUNK_SIZE 16
#define CHUNK_BLOCKS CHUNK_SIZE*CHUNK_SIZE*WORLD_HEIGHT

#define FLOATS_PER_TRIANGLE 15 // defined here because meshs are created here

class World; // forward declaration

class Chunk {
    private:
        int chunkX, chunkZ;
        Block blocks[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE]; // x, y, z
        Mesh* mesh;
        Chunk* neighborNorth; // neighbor in the direction in which x increases
        Chunk* neighborSouth; // neighbor in the direction in which x decreaces
        Chunk* neighborWest; // neighbor in the direction in which z decreases
        Chunk* neighborEast; // neighbor in the direction in which z increases
        void updateNeighbors(World &world);
        Block updateBlock(int x, int y, int z, int chunkX, int chunkZ); // generate block in that coordinates
    public:
        Chunk(int chunkX, int chunkZ);
        int getChunkX();
        int getChunkZ();
        void updateMesh(Atlas a, World &world); // recalculate mesh and update mesh attribute
        Mesh* getMesh(); // get last mesh
        Block getBlock(unsigned int x, unsigned int y, unsigned int z);
};

#endif