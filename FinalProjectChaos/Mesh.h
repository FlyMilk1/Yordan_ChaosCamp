#pragma once
#include <vector>
#include "triangle.h"
#include "Material.h"

class Mesh {
public:
    void addTriangles(std::vector<triangle>& triangleArray);
    void setVertices(const std::vector<vec3>& vecArr);
    void setIndices(const std::vector<int>& indicesArr);
    void setMat(const Material& newMaterial);
    void setUvs(const std::vector<vec3>& uvArr);
    void setOrigin(const vec3& newOrigin);

private:
    std::vector<vec3> vertices;
    std::vector<int> triangleVertIndices;
    std::vector<vec3> uvs;
    vec3 origin;
    Material material;
    
};