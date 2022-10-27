#include "mesh.hpp"
#include <iostream>

Mesh::Mesh() {}

Mesh::Mesh(float* vertices, unsigned int triangles) {
    this->vertices = vertices;
    this->triangles = triangles;
}

Mesh::~Mesh() {
    delete[] this->vertices;
}

float* Mesh::getVertices() {
    return vertices;
}
unsigned int Mesh::getTriangles() {
    return triangles;
}