#include "GPUDefaultHeap.h"

GPUDefaultHeap::GPUDefaultHeap(ID3D12Device* device, UINT verticesCount)
{
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
        verticesCount * sizeof(Vertex)
    );

	HRESULT hr =device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&d3d12Resource)
	);
	assert(SUCCEEDED(hr));
}
