#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <assert.h>
#include <comdef.h>

#define MAKE_SMART_COM_POINTER(_a) _COM_SMARTPTR_TYPEDEF(_a, __uuidof(_a))

MAKE_SMART_COM_POINTER(ID3D12Resource);
MAKE_SMART_COM_POINTER(ID3D12DescriptorHeap);

class DXResource {
public: //Public Functions
	
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT createGPUReadBackHeap(ID3D12Device* device, const DXResource* RenderTargetResource);
	D3D12_RESOURCE_DESC getResourceDescription() const;
	ID3D12Resource* getD3D12Resource() const;
	void cleanUpResource();
protected: //Variables
	ID3D12ResourcePtr d3d12Resource = nullptr;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProperties = {};

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	ID3D12DescriptorHeapPtr descriptorHandle = nullptr;
	

	
};