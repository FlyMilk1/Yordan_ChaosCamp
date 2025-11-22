#pragma once
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "DXResource.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include "CustomStopwatch.h"
#include <QLabel>

MAKE_SMART_COM_POINTER(IDXGIFactory4);
MAKE_SMART_COM_POINTER(IDXGIAdapter1);
MAKE_SMART_COM_POINTER(ID3D12Device);
MAKE_SMART_COM_POINTER(ID3D12CommandQueue);
MAKE_SMART_COM_POINTER(ID3D12CommandAllocator);
MAKE_SMART_COM_POINTER(ID3D12GraphicsCommandList);
MAKE_SMART_COM_POINTER(ID3D12Fence);

static UINT64 FRAME_FENCE_COMPLETION_VALUE = 1;
static UINT RGBA_COLOR_CHANNELS_COUNT = 4;
class DXRenderer {
public: //Public Functions
	/// <summary>
	/// Prepares and renders a frame
	/// </summary>
	/// <param name="RGBAcolor"></param>
	void render(const FLOAT* RGBAcolor);

	/// <summary>
	/// Renders frame but REQUIRES PREPARATION with prepareForRendering() before use
	/// </summary>
	/// <param name="RGBAcolor"></param>
	QImage renderFrame(const FLOAT* RGBAcolor, const bool& writeToFile);
	/// <summary>
	/// Create the necessary DirectX infrastructure and rendering resources
	/// </summary>
	void prepareForRendering(const QLabel* frame);

	/// <summary>
	/// Cleans up memory from rendering variables and pointers
	/// </summary>
	void cleanUp();
private: //Private Functions
	/// <summary>
	/// Create ID3D12Device, an interface for accessing the GPU for use with Direct3D API
	/// </summary>
	void createDevice();

	/// <summary>
	/// Create ID3D12CommandQueue, ID3D12CommandAllocator and ID3D12GraphicsCommandList1 for preparing and passing GPU commands
	/// </summary>
	void createCommandsManager();

	/// <summary>
	/// Create ID3D12Fence used to know when GPU commands are completed
	/// </summary>
	void createFence();

	/// <summary>
	/// Create D3D12_RESOURCE_BARRIER that defines CPU and GPU direction of data transfer
	/// </summary>
	void createBarrier();

	/// <summary>
	/// Changes barrier's direction
	/// </summary>
	/// <param name="direction">FALSE- RT -> Copy Source; TRUE- CopySource->RT</param>
	void flipBarrier(const bool& direction);

	/// <summary>
	/// Create Source and Destination D3D12 Texture Copy Locations
	/// </summary>
	void createSourceDest();

	/// <summary>
	/// Waits for GPU to complete rendering frame
	/// </summary>
	void waitForGPURenderFrame();

	/// <summary>
	/// Writes returned from GPU rendering image to a .ppm file
	/// </summary>
	void writeImageToFile();

	static QImage renderTargetDataToQimage(void* renderTargetData, const UINT64& width, const UINT64& height, const UINT& rowPitch);

private:
	IDXGIFactory4Ptr dxgiFactory = nullptr;
	IDXGIAdapter1Ptr adapter = nullptr;
	ID3D12DevicePtr device = nullptr;

	ID3D12CommandQueuePtr commandQueue = nullptr;
	ID3D12CommandAllocatorPtr commandAllocator = nullptr;
	ID3D12GraphicsCommandListPtr graphicsCommandList = nullptr;

	ID3D12FencePtr frameFence = nullptr;
	HANDLE frameEventHandle = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = {};

	DXResource RTResource;
	DXResource ReadbackResource;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;

	D3D12_RESOURCE_BARRIER barrier;
	D3D12_TEXTURE_COPY_LOCATION source;
	D3D12_TEXTURE_COPY_LOCATION destination;
	
};