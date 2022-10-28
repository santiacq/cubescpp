#include "chunk.hpp"
#include "mesh.hpp"
#include <cstddef>
#include <iostream>

Chunk::Chunk(int chunkX, int chunkZ) {
    this->chunkX = chunkX;
    this->chunkZ = chunkZ;
    this->mesh = NULL;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (y < 4) {
                    blocks[x][y][z] = Block(Stone);
                } else if (y < 6){
                    blocks[x][y][z] = Block(Dirt);
                } else if (y < 7){
                    blocks[x][y][z] = Block(Grass);
                } else {
                    blocks[x][y][z] = Block(Air);
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

void Chunk::updateMesh(Atlas a) { // recalculate mesh and update mesh attribute
    unsigned int triangles = 0;
    // big array because in the worst case, we'll need this amount of floats
    // for each cube, there are six faces, with two triangles each, and FLOATS_PER_TRIANGLE floats each triangle.
    float* tempVertices = new float[CHUNK_BLOCKS*6*2*FLOATS_PER_TRIANGLE];

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // if the block isn't air, add its vertices to mesh
                Block b = blocks[x][y][z];
                if (b.getType() != Air) {
                    float square1[] = {
                        1.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                        1.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side)           ,
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           , a.getV(b, Side)           ,
                        0.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Side)           , a.getV(b, Side) + a.getD(),
                        1.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Side) + a.getD(), a.getV(b, Side) + a.getD(),
                    };
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square1[i];  
                    }
                    triangles += 2;

                    float square2[] = {
                        0.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        1.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        0.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                        0.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                    };
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square2[i];  
                    }
                    triangles += 2;

                    float square3[] = {
                        0.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        0.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        0.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                        0.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                    };
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square3[i];  
                    }
                    triangles += 2;
                    
                    float square4[] = {
                        1.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                        1.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side) + a.getD(),
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side) + a.getD(),
                        1.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Side) + a.getD(),  a.getV(b, Side)           ,
                        1.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Side)           ,  a.getV(b, Side)           ,
                    };
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square4[i];  
                    }
                    triangles += 2;

                    float square5[] = {
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                        1.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom) + a.getD(),
                        1.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                        1.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Bottom) + a.getD() , a.getV(b, Bottom)           ,
                        0.0f + x, 0.0f + y, 1.0f + z, a.getU(b, Bottom)            , a.getV(b, Bottom)           ,
                        0.0f + x, 0.0f + y, 0.0f + z, a.getU(b, Bottom)            , a.getV(b, Bottom) + a.getD(),
                    };  
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square5[i];  
                    }
                    triangles += 2;

                    float square6[] = {
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                        1.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top) + a.getD(),
                        0.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                        0.0f + x, 1.0f + y, 0.0f + z, a.getU(b, Top)           , a.getV(b, Top) + a.getD(),
                        0.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Top)           , a.getV(b, Top)           ,
                        1.0f + x, 1.0f + y, 1.0f + z, a.getU(b, Top) + a.getD(), a.getV(b, Top)           ,
                    };
                    for (unsigned int i = 0; i < 2*FLOATS_PER_TRIANGLE; i++) {
                        tempVertices[triangles*FLOATS_PER_TRIANGLE + i] = square6[i];  
                    }
                    triangles += 2;
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