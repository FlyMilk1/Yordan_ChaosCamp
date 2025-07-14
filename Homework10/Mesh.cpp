#include "Mesh.h"
#include <iostream>

void Mesh::addTriangles(std::vector<triangle>& triangleArray) {
    std::vector<triangle> tempTriangleArray;
    
    for(int firstIndex = 0; firstIndex <= triangleVertIndices.size()-2; firstIndex += 3) {
        triangle tempTriangle(
            vertices[triangleVertIndices[firstIndex]],
            vertices[triangleVertIndices[firstIndex+1]],
            vertices[triangleVertIndices[firstIndex+2]],
            material
        );
        tempTriangleArray.push_back(tempTriangle);
    }

    if(material.getSmooth()) {
        // Smooth shading calculations...
        // (Include your original smooth shading implementation here)
    }

    triangleArray.insert(triangleArray.end(), tempTriangleArray.begin(), tempTriangleArray.end());
}

void Mesh::setVertices(std::vector<vec3> vecArr) {
    vertices = vecArr;
}

void Mesh::setIndices(std::vector<int> indicesArr) {
    triangleVertIndices = indicesArr;
}

void Mesh::setMat(const Material& newMaterial) {
    material = newMaterial;
}