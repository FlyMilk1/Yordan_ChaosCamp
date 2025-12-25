#pragma once
#include "DXResource.h"
class OutputTexture :
    public DXResource
{
public:
    OutputTexture(ID3D12Device* device, const UINT& width, const UINT& height);
};

