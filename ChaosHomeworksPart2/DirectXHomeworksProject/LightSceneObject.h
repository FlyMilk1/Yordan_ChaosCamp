#pragma once
#include "SceneObject.h"
#include "Light.h"
class LightSceneObject :
    public SceneObject
{
    public:
        LightSceneObject(const PointLight& pointLight, const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0), const std::string& name = "Unnamed");
        virtual ~LightSceneObject() = default;

        const PointLight getPointLight() const { return pointLight; }
private:
    PointLight pointLight;
};

