#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <assert.h>
class DXResource {
public: //Public Functions
	void CreateRenderTarget(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE* CPUDescriptorHandle);
private: //Variables
	ID3D12Resource* d3d12Resource = nullptr;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProperties = {};

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	ID3D12DescriptorHeap* descriptorHandle = nullptr;
	

	
};