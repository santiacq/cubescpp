#include "mesh.hpp"
#include <iostream>

Mesh::Mesh() {}

Mesh::Mesh(float* vertices, int triangles) {
    this->vertices = vertices;
    this->triangles = triangles;
}

Mesh::~Mesh() {
    delete[] this->vertices;
}

float* Mesh::getVertices() {
    return vertices;
}
int Mesh::getTriangles() {
    return triangles;
}