#pragma once
#include "DXResource.h"
class GPUReadbackHeapResource :
    public DXResource
{
public:
    GPUReadbackHeapResource(ID3D12Device* device = nullptr, const DXResource* RenderTargetResource = nullptr);
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT getPlacedFootprint() const;
private:
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint = {};
};

