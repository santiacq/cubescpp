#include "chunk.hpp"
#include <cstddef>
#include <iostream>

Chunk::Chunk(int chunkX, int chunkY) {
    this->chunkX = chunkX;
    this->chunkY = chunkY;
    this->mesh = NULL;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (y < 4) {
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

int Chunk::getChunkY() {
    return chunkY;
}

void Chunk::updateMesh() { // recalculate mesh and update mesh attribute
    int triangles = 0;
    // in the worst case, we'll need this amount of floats
    // for each cube, there are six faces, with two triangles each, and 9 floats each triangle.
    float* tempVertices = new float[CHUNK_BLOCKS*6*2*9];

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                float square1[] = {
                    1.0f + x, 1.0f + y, 0.0f + z,
                    1.0f + x, 0.0f + y, 0.0f + z,
                    0.0f + x, 0.0f + y, 0.0f + z,
                    0.0f + x, 0.0f + y, 0.0f + z,
                    0.0f + x, 1.0f + y, 0.0f + z,
                    1.0f + x, 1.0f + y, 0.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square1[i];  
                }
                triangles += 2;

                float square2[] = {
                    0.0f + x, 0.0f + y, 1.0f + z,
                    1.0f + x, 0.0f + y, 1.0f + z, 
                    1.0f + x, 1.0f + y, 1.0f + z, 
                    1.0f + x, 1.0f + y, 1.0f + z, 
                    0.0f + x, 1.0f + y, 1.0f + z, 
                    0.0f + x, 0.0f + y, 1.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square2[i];  
                }
                triangles += 2;

                float square3[] = {
                    0.0f + x, 1.0f + y, 1.0f + z,
                    0.0f + x, 1.0f + y, 0.0f + z,
                    0.0f + x, 0.0f + y, 0.0f + z,
                    0.0f + x, 0.0f + y, 0.0f + z,
                    0.0f + x, 0.0f + y, 1.0f + z,
                    0.0f + x, 1.0f + y, 1.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square3[i];  
                }
                triangles += 2;
                
                float square4[] = {
                    1.0f + x, 0.0f + y, 0.0f + z,
                    1.0f + x, 1.0f + y, 0.0f + z,
                    1.0f + x, 1.0f + y, 1.0f + z,
                    1.0f + x, 1.0f + y, 1.0f + z,
                    1.0f + x, 0.0f + y, 1.0f + z,
                    1.0f + x, 0.0f + y, 0.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square4[i];  
                }
                triangles += 2;

                float square5[] = {
                    0.0f + x, 0.0f + y, 0.0f + z,
                    1.0f + x, 0.0f + y, 0.0f + z,
                    1.0f + x, 0.0f + y, 1.0f + z,
                    1.0f + x, 0.0f + y, 1.0f + z,
                    0.0f + x, 0.0f + y, 1.0f + z,
                    0.0f + x, 0.0f + y, 0.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square5[i];  
                }
                triangles += 2;

                float square6[] = {
                    1.0f + x, 1.0f + y, 1.0f + z,
                    1.0f + x, 1.0f + y, 0.0f + z,
                    0.0f + x, 1.0f + y, 0.0f + z,
                    0.0f + x, 1.0f + y, 0.0f + z,
                    0.0f + x, 1.0f + y, 1.0f + z,
                    1.0f + x, 1.0f + y, 1.0f + z
                };
                for (int i = 0; i < 2*9; i++) {
                    tempVertices[triangles*9 + i] = square6[i];  
                }
                triangles += 2;

            }
        }
    }
    //std::cout << "here 6" << std::endl;
    //std::cout << "triangles: " << triangles << std::endl;
    float* vertices = new float[triangles*9];
    for (int i = 0; i < triangles*9; i++) {
        vertices[i] = tempVertices[i];
    }
    //std::cout << "2 first float: " << vertices[0] << std::endl;
    delete[] tempVertices;
    //std::cout << "2 first float: " << vertices[0] << std::endl;
    delete this->mesh;
    this->mesh = new Mesh(vertices, triangles);
    //std::cout <<  this->mesh.getVertices()[0]  << std::endl;
    //std::cout << "test " << std::endl;
    //std::cout << "8 first float: " << vertices[0] << std::endl;
    //std::cout << "3 first float: " << this->getMesh().getVertices()[0] << std::endl;

}

Mesh* Chunk::getMesh() { // get last mesh
    return mesh;
}