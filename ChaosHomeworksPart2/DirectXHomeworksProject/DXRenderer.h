#pragma once
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "DXResource.h"

#include <assert.h>

#include <fstream>
#include "CustomStopwatch.h"
#include <QLabel>
#include "RTVResource.h"
#include "GPUReadbackHeapResource.h"
#include "VertexBuffer.h"
#include "Shape.h"
#ifdef _DEBUG
	#include <iostream>
	#include "DirectXH.a0236926/x64/Debug/CompiledShaders/ConstColor.hlsl.h"
	#include "DirectXH.a0236926/x64/Debug/CompiledShaders/ConstColorVS.hlsl.h"
#else
	#include "DirectXH.a0236926/x64/Release/CompiledShaders/ConstColor.hlsl.h"
	#include "DirectXH.a0236926/x64/Release/CompiledShaders/ConstColorVS.hlsl.h"
#endif

MAKE_SMART_COM_POINTER(IDXGIFactory4);
MAKE_SMART_COM_POINTER(IDXGIAdapter1);
MAKE_SMART_COM_POINTER(ID3D12Device);
MAKE_SMART_COM_POINTER(ID3D12CommandQueue);
MAKE_SMART_COM_POINTER(ID3D12CommandAllocator);
MAKE_SMART_COM_POINTER(ID3D12GraphicsCommandList);
MAKE_SMART_COM_POINTER(ID3D12Fence);
MAKE_SMART_COM_POINTER(ID3D12Debug);
MAKE_SMART_COM_POINTER(IDXGISwapChain1);
MAKE_SMART_COM_POINTER(IDXGISwapChain3);
MAKE_SMART_COM_POINTER(ID3D12DescriptorHeap);
MAKE_SMART_COM_POINTER(ID3DBlob);
MAKE_SMART_COM_POINTER(ID3D12RootSignature);
MAKE_SMART_COM_POINTER(ID3D12PipelineState);
static UINT RGBA_COLOR_CHANNELS_COUNT = 4;
class DXRenderer {
public: //Public Functions
	DXRenderer();
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
	/// Set the D3D12_RESOURCE_BARRIER that defines CPU and GPU direction of data transfer
	/// </summary>
	void setBarrier(const D3D12_RESOURCE_STATES& beforeState, const D3D12_RESOURCE_STATES& afterState);

	/// <summary>
	/// Create Source and Destination D3D12 Texture Copy Locations
	/// </summary>
	void setSourceDest(const UINT& resourceIndex);

	/// <summary>
	/// Waits for GPU to complete rendering frame
	/// </summary>
	void waitForGPURenderFrame();

	/// <summary>
	/// Writes returned from GPU rendering image to a .ppm file
	/// </summary>
	void writeImageToFile();

	/// <summary>
	/// Converts the renderTargetData into a QImage and returns it
	/// </summary>
	/// <param name="renderTargetData">renderTargetData</param>
	/// <param name="width">Width of the frame</param>
	/// <param name="height">Height of the frame</param>
	/// <param name="rowPitch">Row pitch of render target data</param>
	/// <returns></returns>
	static QImage renderTargetDataToQimage(void* renderTargetData, const UINT64& width, const UINT64& height, const UINT& rowPitch);

	/// <summary>
	/// Creates needed swap chain components for rendering
	/// </summary>
	/// <param name="frame"></param>
	void createSwapChain(const QLabel* frame);

	/// <summary>
	/// Executed in the beggining of renderFrame(). Resets the command allocator and lists, Sets SC buffer Present -> RT.
	/// Clears render target view with RGBA color
	/// </summary>
	/// <param name="RGBAcolor">Clear color</param>
	void frameBegin(const FLOAT* RGBAcolor);

	/// <summary>
	/// Executed in the end of renderFrame(). Sets buffer to RT -> Present. Closes the command list, executes the commands.
	/// Presents the frame;
	/// </summary>
	void frameEnd();

	/// <summary>
	/// Creates the render target views
	/// </summary>
	/// <param name="frame"></param>
	void createRTVs();
	
	/// <summary>
	/// Creates the root signature for shaders
	/// </summary>
	void createRootSignature();

	/// <summary>
	/// Creates the graphics pipeline state
	/// </summary>
	void createPipelineState();

	/// <summary>
	/// Creates the viewport with the output frame's size
	/// </summary>
	/// <param name="frame">Qt output frame</param>
	void createViewport(const QLabel* frame);
private:
	IDXGIFactory4Ptr dxgiFactory = nullptr; //COM Pointer to the DXGI Factory
	IDXGIAdapter1Ptr adapter = nullptr; //COM Pointer to the used for rendering adapter
	ID3D12DevicePtr device = nullptr; //COM Pointer to the used for rendering device

	ID3D12CommandQueuePtr commandQueue = nullptr; //COM Pointer to the command queue
	ID3D12CommandAllocatorPtr commandAllocator = nullptr; //COM Pointer to the command allocator
	ID3D12GraphicsCommandListPtr graphicsCommandList = nullptr; //COM Pointer to the command list

	ID3D12FencePtr frameFence = nullptr; //COM Pointer to the frame fence used to sync CPU and GPU
	HANDLE frameEventHandle = nullptr; //Handle for the frame fence event

	D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = {}; //Handle for the CPU descriptor

	RTVResource RTResource; //Render target resource
	GPUReadbackHeapResource ReadbackResource; //GPU readback heap resource
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint = {}; //D3D12 placed footprint

	D3D12_RESOURCE_BARRIER barrier = {}; //Resource barrier
	D3D12_TEXTURE_COPY_LOCATION source = {}; //Copy location source
	D3D12_TEXTURE_COPY_LOCATION destination = {}; //Copy location destination

	UINT64 renderFrameFenceValue = 0; //Current render frame fence value

	VertexBuffer* vertexBuffer; //Vertex buffer resource
	IDXGISwapChain3Ptr swapChain3; //COM Pointer to the swap chain (3)
	HWND rtvHandle = nullptr; //Handle to the render target view
	UINT currentSwapChainBackBufferIndex = 1; //Current swap chain back buffer index
	ID3D12ResourcePtr backBuffer = nullptr; //COM Pointer to the back buffer
	UINT rtvDescriptorSize = 0; //Render target view descriptor size
	ID3D12DescriptorHeapPtr rtvHeap = nullptr; //COM Pointer to the render target view heap
	
	ID3D12RootSignaturePtr rootSignature; //COM pointer to the root signature
	ID3D12PipelineStatePtr pipelineState; //COM pointer to the pipeline state
	D3D12_VIEWPORT viewport; //Viewport
	D3D12_RECT scissorRect; //Viewport scissor rect

	unsigned int frameIdx; //Current frame index
	bool triangleDirection; //Direction of movement for the triangle

};