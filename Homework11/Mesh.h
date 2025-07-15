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

private:
    std::vector<vec3> vertices;
    std::vector<int> triangleVertIndices;
    Material material;
};