#include "world.hpp"
#include <bits/utility.h>
#include <utility>
#include "../lib/fasthash.h"

void World::updateChunk(int chunkX, int chunkZ) { // generate this chunk
    Chunk* newChunk = new Chunk(chunkX, chunkZ, this);
    chunks.insert({std::make_pair(chunkX, chunkZ), newChunk});
}

World::World(unsigned long seed) {
    this->seed = fasthash(seed);
    this->terrainSeed = seed;
    this->sandSeed = fasthash(seed);
    terrainNoise = siv::PerlinNoise{terrainSeed};
    sandNoise = siv::PerlinNoise{sandSeed};
    // pregenerate some of the world
    for (int x = -20; x <= 20; x++) {
        for (int z = -20; z <= 20; z++) {
            updateChunk(x, z);
        }
    }
}

World::~World() {
    // free memory reserved while updating chunks
    for (auto it = this->chunks.begin(); it != this->chunks.end(); it++) {
        delete it->second;
    }
}

Chunk* World::getChunk(int chunkX, int chunkZ) {
    if (this->chunks.count(std::make_pair(chunkX, chunkZ)) == 0) { // true if chunk isn't in map
        this->updateChunk(chunkX, chunkZ);
    }
    return this->chunks.find(std::make_pair(chunkX, chunkZ))->second;
}

unsigned long World::getSeed() {
    return seed;
}