#pragma once
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "DXResource.h"

#include <assert.h>
#include <iostream>
#include <fstream>
static UINT64 FRAME_FENCE_COMPLETION_VALUE = 1;
static UINT RGBA_COLOR_CHANNELS_COUNT = 4;
class DXRenderer {
public: //Public Functions
	void render(const FLOAT* RGBAcolor);
	void cleanUp();
private: //Private Functions
	/// <summary>
	/// Create the necessary DirectX infrastructure and rendering resources
	/// </summary>
	void prepareForRendering();

	/// <summary>
	/// Create ID3D12Device, an interface for accessing the GPU for use with Direct3D API
	/// </summary>
	void createDevice();

	/// <summary>
	/// Create ID3D12CommandQueue, ID3D12CommandAllocator and ID3D12GraphicsCommandList1 for preparing and passing GPU commands
	/// </summary>
	void createCommandsManager();

	void createFence();

	void createBarrier();

	/// <summary>
	/// Changes barrier's direction
	/// </summary>
	/// <param name="direction">FALSE- RT -> Copy Source; TRUE- CopySource->RT</param>
	void flipBarrier(const bool& direction);

	void createSourceDest();
	/// <summary>
	/// Waits for GPU to complete rendering frame
	/// </summary>
	void waitForGPURenderFrame();

	void writeImageToFile();

	

private:
	IDXGIFactory4* dxgiFactory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	ID3D12Device* device = nullptr;

	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList1* graphicsCommandList = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = {};
	
	ID3D12Fence* frameFence = nullptr;
	HANDLE frameEventHandle = nullptr;

	DXResource RTResource;
	DXResource ReadbackResource;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;

	D3D12_RESOURCE_BARRIER barrier;
	D3D12_TEXTURE_COPY_LOCATION source;
	D3D12_TEXTURE_COPY_LOCATION destination;
	
};