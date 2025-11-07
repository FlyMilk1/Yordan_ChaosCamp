#include "Mesh.h"
#include <iostream>

void Mesh::addTriangles(std::vector<triangle>& triangleArray) {
    std::vector<triangle> tempTriangleArray;
    
	if (uvs.size() > 0) {
		for (int firstIndex = 0; firstIndex <= triangleVertIndices.size() - 2; firstIndex += 3) {
			triangle tempTriangle(
				vertices[triangleVertIndices[firstIndex]],
				vertices[triangleVertIndices[firstIndex + 1]],
				vertices[triangleVertIndices[firstIndex + 2]],
				uvs[triangleVertIndices[firstIndex]],
				uvs[triangleVertIndices[firstIndex + 1]],
				uvs[triangleVertIndices[firstIndex + 2]],
				material
			);
			tempTriangleArray.push_back(tempTriangle);
		}
	}
	else {
		for (int firstIndex = 0; firstIndex <= triangleVertIndices.size() - 2; firstIndex += 3) {
			triangle tempTriangle(
				vertices[triangleVertIndices[firstIndex]],
				vertices[triangleVertIndices[firstIndex + 1]],
				vertices[triangleVertIndices[firstIndex + 2]],
				{0,0,0}, { 0,0,0 }, { 0,0,0 },
				material
			);
			tempTriangleArray.push_back(tempTriangle);
		}
	}
    

    if(material.getSmooth()) {
		for (int trId = 0; trId < tempTriangleArray.size(); trId++) {
			std::vector<vec3> v0Normals;
			std::vector<vec3> v1Normals;
			std::vector<vec3> v2Normals;
			tempTriangleArray[trId].v0N = { 0,0,0 };
			tempTriangleArray[trId].v1N = { 0,0,0 };
			tempTriangleArray[trId].v2N = { 0,0,0 };
			//v0 normals
			for (int checkedTrId = 0; checkedTrId < tempTriangleArray.size(); checkedTrId++) {
				if (tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v0) {
					v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v1) {
					v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v0 == tempTriangleArray[checkedTrId].v2) {
					v0Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
			}
			//v1 normals
			for (int checkedTrId = 0; checkedTrId < tempTriangleArray.size(); checkedTrId++) {
				if (tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v0) {
					v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v1) {
					v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v1 == tempTriangleArray[checkedTrId].v2) {
					v1Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
			}
			//v2 normals
			for (int checkedTrId = 0; checkedTrId < tempTriangleArray.size(); checkedTrId++) {
				if (tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v0) {
					v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v1) {
					v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
				if (tempTriangleArray[trId].v2 == tempTriangleArray[checkedTrId].v2) {
					v2Normals.push_back(tempTriangleArray[checkedTrId].normalVec);
				}
			}

			//v0 normal calculation
			for (int normId = 0; normId < v0Normals.size(); normId++) {
				tempTriangleArray[trId].v0N = tempTriangleArray[trId].v0N + v0Normals[normId];
			}
			tempTriangleArray[trId].v0N = tempTriangleArray[trId].v0N / v0Normals.size();
			//v1 normal calculation
			for (int normId = 0; normId < v1Normals.size(); normId++) {
				tempTriangleArray[trId].v1N = tempTriangleArray[trId].v1N + v1Normals[normId];
			}
			tempTriangleArray[trId].v1N = tempTriangleArray[trId].v1N / v1Normals.size();
			//v2 normal calculation
			for (int normId = 0; normId < v2Normals.size(); normId++) {
				tempTriangleArray[trId].v2N = tempTriangleArray[trId].v2N + v2Normals[normId];
			}
			tempTriangleArray[trId].v2N = tempTriangleArray[trId].v2N / v2Normals.size();
			normalizeVector(tempTriangleArray[trId].v0N);
			normalizeVector(tempTriangleArray[trId].v1N);
			normalizeVector(tempTriangleArray[trId].v2N);

		}
		std::cout << "Calculated vertex normals for mesh\n";
    }

    triangleArray.insert(triangleArray.end(), tempTriangleArray.begin(), tempTriangleArray.end());
}

void Mesh::setVertices(std::vector<vec3> vecArr) {
    vertices = vecArr;
}

void Mesh::setIndices(std::vector<int> indicesArr) {
    triangleVertIndices = indicesArr;
}
void Mesh::setUvs(std::vector<vec3> uvArr) {
	uvs = uvArr;
}
void Mesh::setMat(const Material& newMaterial) {
    material = newMaterial;
}