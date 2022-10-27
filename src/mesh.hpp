#ifndef MESH_HPP
#define MESH_HPP

class Mesh {
    private:
        float* vertices;
        unsigned int triangles;
    public:
        Mesh();
        Mesh(float* vertices, unsigned int triangles);
        ~Mesh();
        float* getVertices();
        unsigned int getTriangles();
};
#endif