#include "world.hpp"
#include <bits/utility.h>
#include <utility>

void World::updateChunk(int chunkX, int chunkZ) { // generate this chunk
    Chunk* newChunk = new Chunk(chunkX, chunkZ, this);
    chunks.insert({std::make_pair(chunkX, chunkZ), newChunk});
}

World::World() {}

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
