#include "DXResource.h"

void DXResource::createRenderTarget(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE* CPUDescriptorHandle, const UINT64& frameWidth, const UINT64& frameHeight)
{
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = frameWidth;
	resourceDesc.Height = frameHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, IID_PPV_ARGS(&d3d12Resource));
	assert(SUCCEEDED(hr));
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHandle));
	assert(SUCCEEDED(hr));
	*CPUDescriptorHandle = descriptorHandle->GetCPUDescriptorHandleForHeapStart();
	device->CreateRenderTargetView(d3d12Resource, nullptr, *CPUDescriptorHandle);
	
}

D3D12_PLACED_SUBRESOURCE_FOOTPRINT DXResource::createGPUReadBackHeap(ID3D12Device* device, const DXResource* RenderTargetResource)
{
	UINT64 readbackBufferSize = 0;
	D3D12_RESOURCE_DESC RTResource = RenderTargetResource->getResourceDescription();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint = {};
	device->GetCopyableFootprints(&RTResource, 0, 1, 0, &placedFootprint, nullptr, nullptr, &readbackBufferSize);
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = readbackBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	heapProperties.Type = D3D12_HEAP_TYPE_READBACK;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&d3d12Resource));
	assert(SUCCEEDED(hr));
	return placedFootprint;
}

D3D12_RESOURCE_DESC DXResource::getResourceDescription() const
{
	return resourceDesc;
}

ID3D12Resource* DXResource::getD3D12Resource() const
{
	return d3d12Resource;
}

void DXResource::cleanUpResource()
{
	if (d3d12Resource) d3d12Resource->Release();
	if (descriptorHandle) descriptorHandle->Release();
}
