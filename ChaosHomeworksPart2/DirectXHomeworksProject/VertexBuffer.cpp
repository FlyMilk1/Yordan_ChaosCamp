#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(ID3D12Device* device)
{
	Vertex triangleVertices[] = {
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f}
	};
	heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(triangleVertices));

	HRESULT hr = device->CreateCommittedResource(&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&d3d12Resource)
	);
	assert(hr);

	void* pVertexData;
	d3d12Resource->Map(0, nullptr, &pVertexData);
	memcpy(pVertexData, triangleVertices, sizeof(triangleVertices));
	d3d12Resource->Unmap(0, nullptr);

	vbView = D3D12_VERTEX_BUFFER_VIEW();
	vbView.BufferLocation = d3d12Resource->GetGPUVirtualAddress();
	vbView.StrideInBytes = sizeof(Vertex);
	vbView.SizeInBytes = sizeof(triangleVertices);
}
