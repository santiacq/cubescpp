#include "chunk.hpp"
#include "block.hpp"
#include "mesh.hpp"
#include <cstddef>
#include <iostream>
#include <math.h>
#include "world.hpp"

void Chunk::updateBlock(int x, int y, int z, Block block, World* world) {
    this->blocks[x][y][z] = block;
    this->isMeshOutdated = true;
    if (x == 0) {
        world->getChunk(this->chunkX - 1, this->chunkZ)->isMeshOutdated = true;
    } else if (x == CHUNK_SIZE - 1) {
        world->getChunk(this->chunkX + 1, this->chunkZ)->isMeshOutdated = true;
    }
    if (z == 0) {
        world->getChunk(this->chunkX, this->chunkZ - 1)->isMeshOutdated = true;
    } else if (z == CHUNK_SIZE - 1) {
        world->getChunk(this->chunkX, this->chunkZ + 1)->isMeshOutdated = true;
    }
}

Chunk::Chunk(int chunkX, int chunkZ, World* world) {
    // setup
    this->chunkX = chunkX;
    this->chunkZ = chunkZ;
    this->opaqueMesh = NULL;
    this->transparentMesh = NULL;
    this->neighborNorth = NULL;
    this->neighborSouth = NULL;
    this->neighborWest = NULL;
    this->neighborEast = NULL;

    // world generation
    Block tempBlocks1[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE]; // temporary block array used for storage of intermadiate chunk generation
    Block tempBlocks2[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE];
    Block tempBlocks3[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE];
    // initial block generation
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                //tempBlocks1[x][y][z] = generateBlock(x, y, z, chunkX, chunkZ, world);
                Block firstGen = Block(Air);
                const double terrainNoise = world->terrainNoise.octave2D_01(((x + CHUNK_SIZE * chunkX) * 0.05), ((z + CHUNK_SIZE * chunkZ) * 0.05), 3);

                int surfaceY = terrainNoise * 20 + 5;
                int waterLevel = 16;
                
                if (y == surfaceY + 2) {
                    if (y >= waterLevel) {
                        firstGen = Block(Grass);
                    } else {
                        firstGen = Block(Dirt);
                    }
                } else if (y < surfaceY) {
                    firstGen = Block(Stone);
                } else if (y < surfaceY + 2) {
                    firstGen = Block(Dirt);
                } else if (y <= waterLevel) {
                    firstGen = Block(Water);
                }
                tempBlocks1[x][y][z] = firstGen;
            }
        }
    }
    // add sand
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                tempBlocks2[x][y][z] = tempBlocks1[x][y][z];

                const bool sandNoise = world->sandNoise.octave2D_01(((x + CHUNK_SIZE * chunkX) * 0.05), ((z + CHUNK_SIZE * chunkZ) * 0.05), 3) > 0.5;

                if (tempBlocks1[x][y][z].getType() != Air && tempBlocks1[x][y][z].getType() != Water && sandNoise) {
                    if (x < CHUNK_SIZE - 1 && tempBlocks1[x + 1][y][z].getType() == Water)
                        tempBlocks2[x][y][z] = Block(Sand);
                    else if (x > 0 && tempBlocks1[x - 1][y][z].getType() == Water)
                        tempBlocks2[x][y][z] = Block(Sand);
                    else if (z < CHUNK_SIZE - 1 && tempBlocks1[x][y][z + 1].getType() == Water)
                        tempBlocks2[x][y][z] = Block(Sand);
                    else if (z > 0 && tempBlocks1[x][y][z - 1].getType() == Water)
                        tempBlocks2[x][y][z] = Block(Sand);
                    else if (y < WORLD_HEIGHT && tempBlocks1[x][y + 1][z].getType() == Water)
                        tempBlocks2[x][y][z] = Block(Sand);
                }
            }
        }
    }
    // use cellular automata logic to expand sand "beaches"
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                tempBlocks3[x][y][z] = tempBlocks2[x][y][z]; 
                if (tempBlocks2[x][y][z].getType() == Water || tempBlocks2[x][y][z].getType() == Grass) {
                    int sandNeighbors = 0;
                    if (x < CHUNK_SIZE - 1 && tempBlocks2[x + 1][y][z].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && tempBlocks2[x - 1][y][z].getType() == Sand)
                        sandNeighbors++;
                    if (z < CHUNK_SIZE - 1 && tempBlocks2[x][y][z + 1].getType() == Sand)
                        sandNeighbors++;
                    if (z > 0 && tempBlocks2[x][y][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (y > 0 && tempBlocks2[x][y - 1][z].getType() == Sand)
                        sandNeighbors++;
                    if (x < CHUNK_SIZE - 1 && z < CHUNK_SIZE - 1 && tempBlocks2[x + 1][y][z + 1].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && z > 0 && tempBlocks2[x - 1][y][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (x < CHUNK_SIZE - 1 && z > 0 && tempBlocks2[x + 1][y][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && z < CHUNK_SIZE - 1 && tempBlocks2[x - 1][y][z + 1].getType() == Sand)
                        sandNeighbors++;
                    
                    if (sandNeighbors >= 3)
                        tempBlocks3[x][y][z] = Block(Sand);
                }
            }
        }
    }
    // add a layer of dirt above the water level
        for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z] = tempBlocks3[x][y][z]; 
                if (tempBlocks2[x][y][z].getType() == Grass || tempBlocks3[x][y][z].getType() == Dirt) {
                    int sandNeighbors = 0;
                    if (x < CHUNK_SIZE - 1 && y > 0 && tempBlocks3[x + 1][y - 1][z].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && y > 0 && tempBlocks3[x - 1][y - 1][z].getType() == Sand)
                        sandNeighbors++;
                    if (z < CHUNK_SIZE - 1 && y > 0 && tempBlocks3[x][y - 1][z + 1].getType() == Sand)
                        sandNeighbors++;
                    if (z > 0 && y > 0 && tempBlocks3[x][y - 1][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (y > 0 && y > 0 && tempBlocks3[x][y - 1][z].getType() == Sand)
                        sandNeighbors++;
                    if (x < CHUNK_SIZE - 1 && z < CHUNK_SIZE - 1 && y > 0 && tempBlocks3[x + 1][y - 1][z + 1].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && z > 0 && y > 0 && tempBlocks3[x - 1][y - 1][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (x < CHUNK_SIZE - 1 && z > 0 && y > 0 && tempBlocks3[x + 1][y - 1][z - 1].getType() == Sand)
                        sandNeighbors++;
                    if (x > 0 && z < CHUNK_SIZE - 1 && y > 0 && tempBlocks3[x - 1][y - 1][z + 1].getType() == Sand)
                        sandNeighbors++;
                    
                    if (sandNeighbors >= 2)
                        blocks[x][y][z] = Block(Sand);
                }
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

void Chunk::updateMeshes(Atlas a, World &world) { // recalculate meshes and update them
    // this could be optimized by keeping track of them separately and updating them in different methods

    unsigned int opaqueTriangles = 0;
    unsigned int transparentTriangles = 0;
    // big array because in the worst case, we'll need this amount of floats
    // for each cube, there are six faces, with two triangles each, and FLOATS_PER_TRIANGLE floats each triangle.
    float* tempOpaqueVertices = new float[CHUNK_BLOCKS*6*2*FLOATS_PER_TRIANGLE];
    float* tempTransparentVertices = new float[CHUNK_BLOCKS*6*2*FLOATS_PER_TRIANGLE];

    updateNeighbors(world); // these are used while culling faces in the chunk limit

    // update opaque mesh
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // if the block isn't air, add its vertices to mesh
                Block b = blocks[x][y][z];
                // only include vertices of non-transparent blocks
                if (b.getType() != Air) {
                    if (b.getType() != Water) { // if the block is opaque work with opaque mesh
                        // only render this face if it isn't facing a solid block
                        if ((z == 0 && (neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() == Air || neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() == Water)) 
                        || (z != 0 && (blocks[x][y][z - 1].getType() == Air || blocks[x][y][z - 1].getType() == Water))) {
                            float square1[] = {
                                0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                                0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side)           ,
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side) + a.getD(),
                                0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                            };
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square1[i];  
                            }
                            opaqueTriangles += 2;
                        }
                        if ((z == (CHUNK_SIZE - 1) && (neighborEast->getBlock(x, y, 0).getType() == Air || neighborEast->getBlock(x, y, 0).getType() == Water))
                        || (z != (CHUNK_SIZE - 1) && (blocks[x][y][z + 1].getType() == Air || blocks[x][y][z + 1].getType() == Water))) {
                            float square2[] = {
                                -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            };
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square2[i];  
                            }
                            opaqueTriangles += 2;
                        }
                        if ((x == 0 && (neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() == Air || neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() == Water))
                        || (x != 0 && (blocks[x - 1][y][z].getType() == Air || blocks[x - 1][y][z].getType() == Water))) {
                            float square3[] = {
                                -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                            };
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square3[i];  
                            }
                            opaqueTriangles += 2;
                        }
                        if ((x == (CHUNK_SIZE - 1) && (neighborNorth->getBlock(0, y, z).getType() == Air || neighborNorth->getBlock(0, y, z).getType() == Water))
                        || (x != (CHUNK_SIZE - 1) && (blocks[x + 1][y][z].getType() == Air || blocks[x + 1][y][z].getType() == Water))) {
                            float square4[] = {
                                0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                            };
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square4[i];  
                            }
                            opaqueTriangles += 2;
                        }
                        if (y == 0 || (blocks[x][y - 1][z].getType() == Air || blocks[x][y - 1][z].getType() == Water)) {
                            float square5[] = {
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                                0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom) + a.getD(),
                                0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom)           ,
                                -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                            };  
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square5[i];  
                            }
                            opaqueTriangles += 2;
                        }
                        if (y == (WORLD_HEIGHT - 1) || (blocks[x][y + 1][z].getType() == Air || blocks[x][y + 1][z].getType() == Water)) {
                            float square6[] = {
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                                0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top) + a.getD(),
                                -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top)           , a.getV(b, Top)           ,
                                0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                            };
                            for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                tempOpaqueVertices[opaqueTriangles*FLOATS_PER_TRIANGLE + i] = square6[i];  
                            }
                            opaqueTriangles += 2;
                            
                        }
                    } else { // if the block is transparent (water) work with transparent mesh

                        if (y == WORLD_HEIGHT - 1 || blocks[x][y + 1][z].getType() != Water) { // make water blocks on top be a bit lower
                            if ((z == 0 && (neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() == Air || neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() != Water)) 
                            || (z != 0 && (blocks[x][y][z - 1].getType() == Air || blocks[x][y][z - 1].getType() != Water))) {
                                float square1[] = {
                                    0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                    -0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square1[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((z == (CHUNK_SIZE - 1) && (neighborEast->getBlock(x, y, 0).getType() == Air || neighborEast->getBlock(x, y, 0).getType() != Water))
                            || (z != (CHUNK_SIZE - 1) && (blocks[x][y][z + 1].getType() == Air || blocks[x][y][z + 1].getType() != Water))) {
                                float square2[] = {
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square2[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((x == 0 && (neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() == Air || neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() != Water))
                            || (x != 0 && (blocks[x - 1][y][z].getType() == Air || blocks[x - 1][y][z].getType() != Water))) {
                                float square3[] = {
                                    -0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    -0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square3[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((x == (CHUNK_SIZE - 1) && (neighborNorth->getBlock(0, y, z).getType() == Air || neighborNorth->getBlock(0, y, z).getType() != Water))
                            || (x != (CHUNK_SIZE - 1) && (blocks[x + 1][y][z].getType() == Air || blocks[x + 1][y][z].getType() != Water))) {
                                float square4[] = {
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square4[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if (y == 0 || (blocks[x][y - 1][z].getType() == Air || blocks[x][y - 1][z].getType() != Water)) {
                                float square5[] = {
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom) + a.getD(),
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                                };  
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square5[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if (y == (WORLD_HEIGHT - 1) || (blocks[x][y + 1][z].getType() == Air || blocks[x][y + 1][z].getType() != Water)) {
                                float square6[] = {
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                                    0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 0.85f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Top)           , a.getV(b, Top)           ,
                                    0.5f + x, 0.85f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square6[i];  
                                }
                                transparentTriangles += 2;
                                
                            }
                        } else { // if its not a top water block
                            if ((z == 0 && (neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() == Air || neighborWest->getBlock(x, y, CHUNK_SIZE - 1).getType() != Water)) 
                            || (z != 0 && (blocks[x][y][z - 1].getType() == Air || blocks[x][y][z - 1].getType() != Water))) {
                                float square1[] = {
                                    0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                                    -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           , a.getV(b, Side) + a.getD(),
                                    0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square1[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((z == (CHUNK_SIZE - 1) && (neighborEast->getBlock(x, y, 0).getType() == Air || neighborEast->getBlock(x, y, 0).getType() != Water))
                            || (z != (CHUNK_SIZE - 1) && (blocks[x][y][z + 1].getType() == Air || blocks[x][y][z + 1].getType() != Water))) {
                                float square2[] = {
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square2[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((x == 0 && (neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() == Air || neighborSouth->getBlock(CHUNK_SIZE - 1, y, z).getType() != Water))
                            || (x != 0 && (blocks[x - 1][y][z].getType() == Air || blocks[x - 1][y][z].getType() != Water))) {
                                float square3[] = {
                                    -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square3[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if ((x == (CHUNK_SIZE - 1) && (neighborNorth->getBlock(0, y, z).getType() == Air || neighborNorth->getBlock(0, y, z).getType() != Water))
                            || (x != (CHUNK_SIZE - 1) && (blocks[x + 1][y][z].getType() == Air || blocks[x + 1][y][z].getType() != Water))) {
                                float square4[] = {
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                    0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square4[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if (y == 0 || (blocks[x][y - 1][z].getType() == Air || blocks[x][y - 1][z].getType() != Water)) {
                                float square5[] = {
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                                    0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom) + a.getD(),
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                    0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                                    -0.5f + x, 0.0f + y, 0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom)           ,
                                    -0.5f + x, 0.0f + y, -0.5f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                                };  
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square5[i];  
                                }
                                transparentTriangles += 2;
                            }
                            if (y == (WORLD_HEIGHT - 1) || (blocks[x][y + 1][z].getType() == Air || blocks[x][y + 1][z].getType() != Water)) {
                                float square6[] = {
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                                    0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 1.0f + y, -0.5f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                                    -0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top)           , a.getV(b, Top)           ,
                                    0.5f + x, 1.0f + y, 0.5f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                                };
                                for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                                    tempTransparentVertices[transparentTriangles*FLOATS_PER_TRIANGLE + i] = square6[i];  
                                }
                                transparentTriangles += 2;
                                
                            }
                        }
                        
                    }
                }
            }
        }
    }
    
    // opaque mesh
    // create a new array with buffers with the right size
    float* opaqueVertices = new float[opaqueTriangles*FLOATS_PER_TRIANGLE];
    for (unsigned int i = 0; i < opaqueTriangles*FLOATS_PER_TRIANGLE; i++) {
        opaqueVertices[i] = tempOpaqueVertices[i];
    }
    // delete temporary big array
    delete[] tempOpaqueVertices;
    // delete old mesh
    delete this->opaqueMesh;
    // create new mesh with the new array
    this->opaqueMesh = new Mesh(opaqueVertices, opaqueTriangles);

    // transparent mesh
    float* transparentVertices = new float[transparentTriangles*FLOATS_PER_TRIANGLE];
    for (unsigned int i = 0; i < transparentTriangles*FLOATS_PER_TRIANGLE; i++) {
        transparentVertices[i] = tempTransparentVertices[i];
    }
    delete[] tempTransparentVertices;
    delete this->transparentMesh;
    this->transparentMesh = new Mesh(transparentVertices, transparentTriangles);

    this->isMeshOutdated = false;
}

Mesh* Chunk::getOpaqueMesh() { // get last mesh
    return opaqueMesh;
}

Mesh* Chunk::getTransparentMesh() {
    return transparentMesh;
}

Block Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z) {
    return blocks[x][y][z];
}