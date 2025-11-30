#include "RTVResource.h"

RTVResource::RTVResource(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE* outFirstRTVHandle, const UINT64& frameWidth, const UINT64& frameHeight)
{
	if (!device || !outFirstRTVHandle || frameHeight == 0 || frameWidth == 0) return;
	resourceDesc = D3D12_RESOURCE_DESC();
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = frameWidth;
	resourceDesc.Height = frameHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	descriptorHeapDesc.NumDescriptors = 2;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHandle));
	assert(SUCCEEDED(hr));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHandle->GetCPUDescriptorHandleForHeapStart();
	*outFirstRTVHandle = handle;

	UINT increment = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT rtvIndex = 0; rtvIndex < 2; ++rtvIndex) {
		hr = device->CreateCommittedResource(
			&heapProperties, 
			D3D12_HEAP_FLAG_NONE, 
			&resourceDesc, 
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			nullptr, 
			IID_PPV_ARGS(&rtvResources[rtvIndex]));
		assert(SUCCEEDED(hr));
		device->CreateRenderTargetView(rtvResources[rtvIndex], nullptr, handle);

		handle.ptr += increment;
	}
}

const ID3D12ResourcePtr RTVResource::getRTVResource(UINT resourceIndex) const
{
	return rtvResources[resourceIndex];
}
