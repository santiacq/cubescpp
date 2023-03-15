#ifndef WORLD_HPP
#define WORLD_HPP

#include <map>
#include "chunk.hpp"
#include "../lib/PerlinNoise.hpp"

class World {
    private:
        const siv::PerlinNoise::seed_type terrainSeed = 435435345433845534u;
        const siv::PerlinNoise::seed_type sandSeed = 1243534586346533456u;

        std::map<std::pair<int, int>, Chunk*> chunks; // the key pair is chunkX and ChunkZ
        void updateChunk(int chunkX, int chunkZ); // generate this chunk
    public:
        const siv::PerlinNoise terrainNoise{terrainSeed};
        const siv::PerlinNoise sandNoise{sandSeed};

        World();
        ~World();
        Chunk* getChunk(int chunkX, int chunkZ);
};

#endif