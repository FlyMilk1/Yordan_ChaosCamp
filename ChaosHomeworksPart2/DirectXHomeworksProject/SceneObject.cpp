#include "SceneObject.h"

SceneObject::SceneObject(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const std::string& name) : position(position), rotation(rotation), name(name) {}

void SceneObject::setPosition(const DirectX::XMFLOAT3& position)
{
	this->position = position;
}

void SceneObject::setRotation(const DirectX::XMFLOAT3& rotation)
{
	this->rotation = rotation;
}

DirectX::XMFLOAT3 SceneObject::getPosition() const
{
	return position;
}

DirectX::XMFLOAT3 SceneObject::getRotation() const
{
	return rotation;
}
