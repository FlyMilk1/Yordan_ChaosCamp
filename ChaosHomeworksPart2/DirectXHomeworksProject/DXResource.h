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

	/// <summary>
	/// Returns the resource description
	/// </summary>
	D3D12_RESOURCE_DESC getResourceDescription() const;

	/// <summary>
	/// Returns a pointer to the D3D12 resource
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* getD3D12Resource() const;

	/// <summary>
	/// Cleans the resource from memory
	/// </summary>
	void cleanUpResource();
protected: //Variables
	ID3D12ResourcePtr d3d12Resource = nullptr; //Pointer to the D3D12 resource
	D3D12_RESOURCE_DESC resourceDesc = {}; //D3D12 Resource description
	D3D12_HEAP_PROPERTIES heapProperties = {}; //D3D12 Heap properties

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {}; //D3D12 Descriptor heap description
	ID3D12DescriptorHeapPtr descriptorHandle = nullptr; //Handle for the descriptor
	

	
};