#pragma once
#include "DXResource.h"
#include "Light.h"
#include <vector>
class LightResource :
    public DXResource
{
public:
    LightResource(ID3D12Device* device);
    virtual ~LightResource() = default;

    void updateLights(const std::vector<PointLight> pointLights);
    const UINT getPointLightsCount() const { return pointLights.size(); };
private:
    std::vector<PointLight> pointLights;
};

