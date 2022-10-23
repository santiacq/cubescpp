#include "block.hpp"
#include "mesh.hpp"

#define WORLD_HEIGHT 128
#define CHUNK_SIZE 16
#define CHUNK_BLOCKS CHUNK_SIZE*CHUNK_SIZE*WORLD_HEIGHT

class Chunk {
    private:
        int chunkX, chunkY;
        Block blocks[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE]; // x, y, z
        Mesh* mesh;
    public:
        Chunk(int x, int y);
        int getChunkX();
        int getChunkY();
        void updateMesh(); // recalculate mesh and update mesh attribute
        Mesh* getMesh(); // get last mesh
};