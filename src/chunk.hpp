#include "block.hpp"
#include "mesh.hpp"
#include "atlas.hpp"

#define WORLD_HEIGHT 128
#define CHUNK_SIZE 16
#define CHUNK_BLOCKS CHUNK_SIZE*CHUNK_SIZE*WORLD_HEIGHT

#define FLOATS_PER_TRIANGLE 15 // defined here because meshs are created here

class Chunk {
    private:
        int chunkX, chunkZ;
        Block blocks[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE]; // x, y, z
        Mesh* mesh;
    public:
        Chunk(int chunkX, int chunkZ);
        int getChunkX();
        int getChunkZ();
        void updateMesh(Atlas a); // recalculate mesh and update mesh attribute
        Mesh* getMesh(); // get last mesh
};