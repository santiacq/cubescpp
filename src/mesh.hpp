#ifndef MESH_HPP
#define MESH_HPP

class Mesh {
    private:
        float* vertices;
        int triangles;
    public:
        Mesh();
        Mesh(float* vertices,int triangles);
        ~Mesh();
        float* getVertices();
        int getTriangles();
};
#endif