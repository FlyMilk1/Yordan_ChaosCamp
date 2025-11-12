#include "DXResource.h"

void DXResource::CreateRenderTarget(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE* CPUDescriptorHandle)
{
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = 3;
	resourceDesc.Height = 2;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr, IID_PPV_ARGS(&d3d12Resource));
	assert(SUCCEEDED(hr));
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHandle));
	assert(SUCCEEDED(hr));
	*CPUDescriptorHandle = descriptorHandle->GetCPUDescriptorHandleForHeapStart();
	device->CreateRenderTargetView(d3d12Resource, nullptr, *CPUDescriptorHandle);
	
}
