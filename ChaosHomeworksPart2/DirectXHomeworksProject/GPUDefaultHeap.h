#pragma once
#include "DXResource.h"
#include <vector>
#include "Vertex.h"
class GPUDefaultHeap : public DXResource
{
public:
    GPUDefaultHeap(ID3D12Device* device, UINT verticesCount);
};

