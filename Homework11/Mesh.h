#pragma once
#include <vector>
#include "triangle.h"
#include "Material.h"

class Mesh {
public:
    void addTriangles(std::vector<triangle>& triangleArray);
    void setVertices(std::vector<vec3> vecArr);
    void setIndices(std::vector<int> indicesArr);
    void setMat(const Material& newMaterial);
    void setUvs(std::vector<vec3> uvArr);

private:
    std::vector<vec3> vertices;
    std::vector<int> triangleVertIndices;
    std::vector<vec3> uvs;
    Material material;
};