#ifndef WORLD_HPP
#define WORLD_HPP

#include <map>
#include "chunk.hpp"

class World {
    private:
        std::map<std::pair<int, int>, Chunk*> chunks; // the key pair is chunkX and ChunkZ
        void updateChunk(int chunkX, int chunkZ); // generate this chunk
    public:
        World();
        ~World();
        Chunk* getChunk(int chunkX, int chunkZ);
};

#endif