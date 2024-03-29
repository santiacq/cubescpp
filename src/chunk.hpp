#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "block.hpp"
#include "mesh.hpp"
#include "atlas.hpp"

#define WORLD_HEIGHT 128
#define CHUNK_SIZE 16
#define CHUNK_BLOCKS CHUNK_SIZE*CHUNK_SIZE*WORLD_HEIGHT

#define FLOATS_PER_TRIANGLE 18 // defined here because meshs are created here

class World; // forward declaration

class Chunk {
    private:
        int chunkX, chunkZ;
        Block blocks[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE]; // x, y, z
        Mesh* opaqueMesh;
        Mesh* transparentMesh;
        Chunk* neighborNorth; // neighbor in the direction in which x increases
        Chunk* neighborSouth; // neighbor in the direction in which x decreaces
        Chunk* neighborWest; // neighbor in the direction in which z decreases
        Chunk* neighborEast; // neighbor in the direction in which z increases
        void updateNeighbors(World &world);
    public:
        Chunk(int chunkX, int chunkZ, World* world);
        int getChunkX();
        int getChunkZ();
        void updateMeshes(Atlas a, World &world); // recalculate mesh and update mesh attribute
        bool isMeshOutdated; // could separate this for opaque and transparent meshes to optimize
        Mesh* getOpaqueMesh(); // get last mesh
        Mesh* getTransparentMesh();
        Block getBlock(unsigned int x, unsigned int y, unsigned int z);
        void updateBlock(int x, int y, int z, Block block, World* world);
};

#endif