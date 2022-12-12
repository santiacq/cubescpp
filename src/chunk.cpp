#include "chunk.hpp"
#include "mesh.hpp"
#include <cstddef>
#include <iostream>
#include <math.h>
#include "world.hpp"

Block Chunk::updateBlock(int x, int y, int z, int chunkX, int chunkZ) {
    float frequency = 0.2;
    float amplitude = 4;
    int surfaceY = 10 + sin((x + chunkX*16)*frequency)*amplitude + sin((z + chunkZ*16)*frequency)*amplitude;
    if (y < surfaceY) {
        return Block(Grass);
    } else if (y < 8) {
         return Block(Water);
    } else { 
        return Block(Air);
    }
}

Chunk::Chunk(int chunkX, int chunkZ) {
    this->chunkX = chunkX;
    this->chunkZ = chunkZ;
    this->mesh = NULL;
    this->neighborNorth = NULL;
    this->neighborSouth = NULL;
    this->neighborWest = NULL;
    this->neighborEast = NULL;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z] = updateBlock(x, y, z, chunkX, chunkZ);       
            }
        }
    }
}

int Chunk::getChunkX() {
    return chunkX;
}

int Chunk::getChunkZ() {
    return chunkZ;
}

void Chunk::updateNeighbors(World &world) {
    if (this->neighborNorth == NULL) {
        this->neighborNorth = world.getChunk(this->getChunkX() + 1, this->getChunkZ());
    }
    if (this->neighborSouth == NULL) {
        this->neighborSouth = world.getChunk(this->getChunkX() - 1, this->getChunkZ());
    }
    if (this->neighborWest == NULL) {
        this->neighborWest = world.getChunk(this->getChunkX(), this->getChunkZ() - 1);
    }
    if (this->neighborEast == NULL) {
        this->neighborEast = world.getChunk(this->getChunkX(), this->getChunkZ() + 1);
    }
}

void Chunk::updateMesh(Atlas a, World &world) { // recalculate mesh and update mesh attribute
    unsigned int triangles = 0;
    // big array because in the worst case, we'll need this amount of floats
    // for each cube, there are six faces, with two triangles each, and FLOATS_PER_TRIANGLE floats each triangle.
    float* tempVertices = new float[CHUNK_BLOCKS*6*2*FLOATS_PER_TRIANGLE];

    updateNeighbors(world); // these are used while culling faces in the chunk limit

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // if the block isn't air, add its vertices to mesh
                Block b = blocks[x][y][z];
                if (b.getType() != Air) {
                    // only render this face if it isn't facing a solid block
                    if ((z == 0 && neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() == Air) || (z != 0 && blocks[x][y][z - 1].getType() == Air)) {
                        float square1[] = {
                            0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                            0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side)           ,
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                            -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side) + a.getD(),
                            0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                        };
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square1[i];  
                        }
                        triangles += 2;
                    }
                    if ((z == (CHUNK_SIZE - 1) && neighborEast->getBlock(x, y, 0).getType() == Air) || (z != (CHUNK_SIZE - 1) && blocks[x][y][z + 1].getType() == Air)) {
                        float square2[] = {
                            -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                            -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        };
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square2[i];  
                        }
                        triangles += 2;
                    }
                    if ((x == 0 && neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() == Air) || (x != 0 && blocks[x - 1][y][z].getType() == Air)) {
                        float square3[] = {
                            -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                            -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        };
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square3[i];  
                        }
                        triangles += 2;
                    }
                    if ((x == (CHUNK_SIZE - 1) && neighborNorth->getBlock(0, y, z).getType() == Air) || (x != (CHUNK_SIZE - 1) && blocks[x + 1][y][z].getType() == Air)) {
                        float square4[] = {
                            0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                            0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        };
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square4[i];  
                        }
                        triangles += 2;
                    }
                    if (y == 0 || blocks[x][y - 1][z].getType() == Air) {
                        float square5[] = {
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                            0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom) + a.getD(),
                            0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                            0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                            -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom)           ,
                            -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                        };  
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square5[i];  
                        }
                        triangles += 2;
                    }
                    if (y == (WORLD_HEIGHT - 1) || blocks[x][y + 1][z].getType() == Air) {
                        float square6[] = {
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                            0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top) + a.getD(),
                            -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                            -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                            -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top)           , a.getV(b, Top)           ,
                            0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                        };
                        for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                            tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square6[i];  
                        }
                        triangles += 2;
                    }
                }
            }
        }
    }
    
    // create a new array with buffers with the right size
    float* vertices = new float[triangles*FLOATS_PER_TRIANGLE];
    for (unsigned int i = 0; i < triangles*FLOATS_PER_TRIANGLE; i++) {
        vertices[i] = tempVertices[i];
    }
    // delete temporary big array
    delete[] tempVertices;
    // delete old mesh
    delete this->mesh;
    // create new mesh with the new array
    this->mesh = new Mesh(vertices, triangles);
}

Mesh* Chunk::getMesh() { // get last mesh
    return mesh;
}

Block Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z) {
    return blocks[x][y][z];
}