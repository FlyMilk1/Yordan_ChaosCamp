#pragma once
#include "DXResource.h"
class SBTDefaultHeap :
    public DXResource
{
    public: //Public Functions
    SBTDefaultHeap(ID3D12Device* device, const UINT& size);
	~SBTDefaultHeap() override = default;
};

