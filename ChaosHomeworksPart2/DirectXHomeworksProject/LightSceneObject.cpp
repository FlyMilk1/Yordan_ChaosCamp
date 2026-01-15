#include "LightSceneObject.h"

LightSceneObject::LightSceneObject(const PointLight& pointLight, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const std::string& name)
{
	this->position = position;
	this->rotation = rotation;
	this->type = LIGHT;
	this->name = name;
	this->pointLight = pointLight;
}
