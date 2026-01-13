#include "GeometrySceneObject.h"

GeometrySceneObject::GeometrySceneObject(const Mesh& mesh, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const std::string& name)
{
	this->mesh = mesh;
	this->position = position;
	this->rotation = rotation;
	this->type = GEOMETRY;
	this->name = name;
}

std::vector<Vertex> GeometrySceneObject::getVertices() const
{
	std::vector<Vertex> verticesAppliedOffset;
	for (const auto& vertex : mesh.vertices)
	{
		verticesAppliedOffset.push_back(Vertex{
			vertex.x + position.x,
			vertex.y + position.y,
			vertex.z + position.z
			});
	}
	return verticesAppliedOffset;
}
