#include "VertexBuffer.h"
#include <iostream>
VertexBuffer::VertexBuffer(ID3D12Device* device) : device(device)
{
    if (triangleVertices.empty()) {
        return;
    }
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
        triangleVertices.size() * sizeof(Vertex)
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

    void* pVertexData;
    d3d12Resource->Map(0, nullptr, &pVertexData);
    memcpy(pVertexData, triangleVertices.data(),
        triangleVertices.size() * sizeof(Vertex));
    d3d12Resource->Unmap(0, nullptr);

    vbView.BufferLocation = d3d12Resource->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(Vertex);
    vbView.SizeInBytes = triangleVertices.size() * sizeof(Vertex);
}

VertexBuffer::~VertexBuffer()
{

}


const D3D12_VERTEX_BUFFER_VIEW* VertexBuffer::getVertexBufferViewPointer() const
{
	return &vbView;
}

void VertexBuffer::moveTriangle(const unsigned int& triangleIndex, const float& right, const float& up)
{
	triangleVertices[triangleIndex].x += right;
	triangleVertices[triangleIndex+1].x += right;
	triangleVertices[triangleIndex+2].x += right;

	triangleVertices[triangleIndex+0].y += up;
	triangleVertices[triangleIndex+1].y += up;
	triangleVertices[triangleIndex+2].y += up;
}
void VertexBuffer::RotateVertex(Vertex& v, const float& angle, const Vertex& center)
{
	float s = sinf(angle);
	float c = cosf(angle);

	float x = v.x - center.x;
	float y = v.y - center.y;

	float xr = x * c - y * s;
	float yr = x * s + y * c;

	v.x = center.x + xr;
	v.y = center.y + yr;
}

void VertexBuffer::updateTriangles()
{
    resizeBufferIfNeeded();
    void* pVertexData;
    HRESULT hr = d3d12Resource->Map(0, nullptr, &pVertexData);
    assert(SUCCEEDED(hr));
    assert(pVertexData);
    memcpy(pVertexData, triangleVertices.data(), triangleVertices.size() * sizeof(Vertex));
    d3d12Resource->Unmap(0, nullptr);
}

void VertexBuffer::resizeBufferIfNeeded()
{
    size_t requiredSize = triangleVertices.size() * sizeof(Vertex);

    if (requiredSize <= vbView.SizeInBytes)
        return; // enough space, nothing to do

    
    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);

    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    );
    assert(SUCCEEDED(hr));

    vbView.BufferLocation = d3d12Resource->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(Vertex);
    vbView.SizeInBytes = requiredSize;
}

unsigned int VertexBuffer::addVerticesToBuffer(const std::vector<Vertex>& vertices)
{
    unsigned int firstIndex = triangleVertices.size();
    triangleVertices.insert(triangleVertices.end(), vertices.begin(), vertices.end());
    return firstIndex;
}

const unsigned int VertexBuffer::getVerticesCount() const
{
    return triangleVertices.size();
}

void VertexBuffer::rotateTriangle(const unsigned int& triangleIndex, const float& angle)
{
	float cx = (triangleVertices[triangleIndex].x + triangleVertices[triangleIndex+1].x + triangleVertices[triangleIndex+2].x) / 3.0f;
	float cy = (triangleVertices[triangleIndex].y + triangleVertices[triangleIndex+1].y + triangleVertices[triangleIndex+2].y) / 3.0f;

    RotateVertex(triangleVertices[triangleIndex], angle, { cx, cy });
    RotateVertex(triangleVertices[triangleIndex + 1], angle, { cx, cy });
    RotateVertex(triangleVertices[triangleIndex + 2], angle, { cx, cy });
}

