#pragma once
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <assert.h>
#include <iostream>
class DXRenderer {
public: //Public Functions
	void render();
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

private:
	IDXGIFactory4* dxgiFactory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	ID3D12Device* device = nullptr;

	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList1* graphicsCommandList = nullptr;
};