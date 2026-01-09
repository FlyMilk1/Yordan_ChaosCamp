#include "CameraBufferResource.h"

CameraBufferResource::CameraBufferResource(ID3D12Device* device)
{
    UINT cbSize = (sizeof(CameraCB) + 255) & ~255;

    heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    );
	assert(SUCCEEDED(hr));

	cameraData.cameraPosition = DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f);

	void* pData = nullptr;
	hr = d3d12Resource->Map(0, nullptr, &pData);
	memcpy(pData, &cameraData, sizeof(CameraCB));
	d3d12Resource->Unmap(0, nullptr);
}
