#pragma once
#include "DXResource.h"
class RTVResource :
    public DXResource
{
public:
        RTVResource(ID3D12Device* device = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE* CPUDescriptorHandle = nullptr, const UINT64& frameWidth = 0, const UINT64& frameHeight = 0);

    const ID3D12ResourcePtr getRTVResource(UINT resourceIndex) const;
private:
    ID3D12ResourcePtr rtvResources[2];
};

