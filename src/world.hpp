#ifndef WORLD_HPP
#define WORLD_HPP

#include <map>
#include "chunk.hpp"
#include "../lib/PerlinNoise.hpp"

class World {
    private:
        unsigned long seed;
        siv::PerlinNoise::seed_type terrainSeed;
        siv::PerlinNoise::seed_type sandSeed;

        std::map<std::pair<int, int>, Chunk*> chunks; // the key pair is chunkX and ChunkZ
        void updateChunk(int chunkX, int chunkZ); // generate this chunk
    public:
        siv::PerlinNoise terrainNoise;
        siv::PerlinNoise sandNoise;

        World(unsigned long seed);
        ~World();
        Chunk* getChunk(int chunkX, int chunkZ);
        unsigned long getSeed();
};

#endif