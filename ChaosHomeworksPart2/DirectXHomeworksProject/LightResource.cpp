#include "LightResource.h"

LightResource::LightResource(ID3D12Device* device)
{
    pointLights.push_back({ {0,0,0}, 0 });
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
        pointLights.size() * sizeof(PointLight)
    );

    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    );
    assert(SUCCEEDED(hr));

    void* pLightPositionsData;
    d3d12Resource->Map(0, nullptr, &pLightPositionsData);
    memcpy(pLightPositionsData, pointLights.data(),
        pointLights.size() * sizeof(PointLight));
    d3d12Resource->Unmap(0, nullptr);

}

void LightResource::updateLights(const std::vector<PointLight> pointLights)
{
    this->pointLights = pointLights;
    void* pLightPositionsData;
    d3d12Resource->Map(0, nullptr, &pLightPositionsData);
    memcpy(pLightPositionsData, pointLights.data(),
        pointLights.size() * sizeof(PointLight));
    d3d12Resource->Unmap(0, nullptr);
}
