#pragma once
#include "DXResource.h"
#include "Vertex.h"
class VertexBuffer :
    public DXResource
{
    VertexBuffer(ID3D12Device* device);
public:

private:
    D3D12_VERTEX_BUFFER_VIEW vbView;
};

