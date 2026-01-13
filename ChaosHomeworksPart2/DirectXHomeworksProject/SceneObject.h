#pragma once
#include <DirectXMath.h>
#include <string>
enum ObjectType
{
    GENERIC,
    CAMERA,
    LIGHT,
	GEOMETRY
};
class SceneObject
{
public:
    SceneObject(const DirectX::XMFLOAT3& position=DirectX::XMFLOAT3(0,0,0), const DirectX::XMFLOAT3& rotation=DirectX::XMFLOAT3(0,0,0), const std::string& name="Unnamed");
    virtual ~SceneObject() = default;

    virtual void setPosition(const DirectX::XMFLOAT3& position);
    virtual void setRotation(const DirectX::XMFLOAT3& rotation);

    virtual DirectX::XMFLOAT3 getPosition() const;
    virtual DirectX::XMFLOAT3 getRotation() const;

	const ObjectType getType() const { return type; }
	const std::string& getName() const { return name; }
protected:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
	ObjectType type = GENERIC;
	std::string name = "Unnamed";
};

