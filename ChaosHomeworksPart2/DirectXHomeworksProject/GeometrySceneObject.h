#pragma once
#include "SceneObject.h"
#include "Vertex.h"
#include <vector>
struct Material {
    DirectX::XMFLOAT3 albedo = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float reflectivity = 0.0f;
    float refractiveIndex = 1.0f;
    float roughness = 0.0f;
};
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
};
class GeometrySceneObject :
    public SceneObject
{
public:
		GeometrySceneObject(const Mesh& mesh, const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0), const std::string& name = "Unnamed");

        /// <summary>
		/// Returns the vertices of the geometry with the object's position applied as an offset.
        /// </summary>
        /// <returns></returns>
        std::vector<Vertex> getVertices() const;

		/// <summary>
		/// Returns the indices of the geometry.
		/// </summary>
		/// <returns></returns>
		std::vector<unsigned int> getIndices() const { return mesh.indices; }
private:
	Mesh mesh;
};

