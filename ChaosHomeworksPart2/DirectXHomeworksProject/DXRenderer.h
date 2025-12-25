#pragma once
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "d3dx12_default.h"
#include "d3dx12_core.h"
#include "d3dx12_root_signature.h"
#include "DXResource.h"

#include <assert.h>

#include <fstream>
#include "CustomStopwatch.h"
#include <QLabel>
#include "RTVResource.h"
#include "GPUReadbackHeapResource.h"
#include "VertexBuffer.h"
#include "Shape.h"
#include "GPUDefaultHeap.h"
#include "FrameData.h"
#include "OutputTexture.h"
#include "SBTUploadHeap.h"
#include "SBTDefaultHeap.h"

#include "CompiledShaders/ConstColor.hlsl.h"
#include "CompiledShaders/ConstColorVS.hlsl.h"


MAKE_SMART_COM_POINTER(IDXGIFactory4);
MAKE_SMART_COM_POINTER(IDXGIAdapter1);
MAKE_SMART_COM_POINTER(ID3D12Device5);
MAKE_SMART_COM_POINTER(ID3D12CommandQueue);
MAKE_SMART_COM_POINTER(ID3D12CommandAllocator);
MAKE_SMART_COM_POINTER(ID3D12GraphicsCommandList4);
MAKE_SMART_COM_POINTER(ID3D12Fence);
MAKE_SMART_COM_POINTER(ID3D12Debug);
MAKE_SMART_COM_POINTER(IDXGISwapChain1);
MAKE_SMART_COM_POINTER(IDXGISwapChain3);
MAKE_SMART_COM_POINTER(ID3D12DescriptorHeap);
MAKE_SMART_COM_POINTER(ID3DBlob);
MAKE_SMART_COM_POINTER(ID3D12RootSignature);
MAKE_SMART_COM_POINTER(ID3D12PipelineState);
MAKE_SMART_COM_POINTER(ID3D12StateObject);
MAKE_SMART_COM_POINTER(ID3D12StateObjectProperties);

static UINT RGBA_COLOR_CHANNELS_COUNT = 4;
static inline UINT alignedSize(UINT size, UINT alignBytes);

class DXRenderer {
public: //Public Functions
	DXRenderer();

	/// <summary>
	/// Renders frame but REQUIRES PREPARATION with prepareForRendering() before use
	/// </summary>
	/// <param name="frameData">Frame Data for shaders</param>
	/// <param name="writeToFile">Should write output frame to a .ppm file</param>
	/// <returns></returns>
	QImage renderFrame(const FrameData& frameData, const bool& writeToFile);
	/// <summary>
	/// Create the necessary DirectX infrastructure and rendering resources
	/// </summary>
	void prepareForRendering(const QLabel* frame);

	/// <summary>
	/// Returns a color based on input
	/// </summary>
	/// <param name="i">Input integer</param>
	/// <param name="out">Array to write output on</param>
	void getFrameColor(int i, float out[3]);

	/// <summary>
	/// Cleans up memory from rendering variables and pointers
	/// </summary>
	void cleanUp();

	/// <summary>
	/// Checks whether ray tracing is supported on the current system or environment.
	/// </summary>
	void checkRayTracingSupport();

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

	/// <summary>
	/// Executes beforex creating the vertex buffer.
	/// </summary>
	void createTriangles();

	/// <summary>
	/// Creates the vertex buffer and the upload and default heap
	/// </summary>
	void createVertexBuffer();

	/// <summary>
	/// Create the 2D output texture for RT
	/// </summary>
	/// <param name="frame">Qt output frame</param>
	void createOutputTexture(const QLabel* frame);

	/// <summary>
	/// Create a Global Root Signature for RT
	/// </summary>
	void createGlobalRootSignature();

	/// <summary>
	/// Creates a D3D12_STATE_SUBOBJECT configured as a ray-generation shader library for a Direct3D 12 raytracing state object.
	/// </summary>
	/// <returns>A D3D12_STATE_SUBOBJECT representing the ray-generation library subobject, ready to be included in a raytracing state object.</returns>
	D3D12_STATE_SUBOBJECT createRayGenLibSubObject();

	/// <summary>
	/// Creates a D3D12_STATE_SUBOBJECT configured as a miss shader library subobject for a Direct3D 12 ray-tracing state object.
	/// </summary>
	/// <returns>A D3D12_STATE_SUBOBJECT value representing the miss shader library subobject, ready to be included in a ray-tracing state object.</returns>
	D3D12_STATE_SUBOBJECT createMissShaderLibSubObject();

	/// <summary>
	/// Creates a Direct3D 12 ray tracing shader configuration subobject.
	/// </summary>
	/// <returns>A D3D12_STATE_SUBOBJECT initialized as a ray tracing shader configuration subobject, suitable for inclusion in a Direct3D 12 state object.</returns>
	D3D12_STATE_SUBOBJECT createRayTracingShaderConfigSubObject();

	/// <summary>
	/// Creates a D3D12_STATE_SUBOBJECT that represents pipeline configuration settings for a Direct3D 12 pipeline.
	/// </summary>
	/// <returns>A D3D12_STATE_SUBOBJECT configured as a pipeline configuration subobject.</returns>
	D3D12_STATE_SUBOBJECT createPipelineConfigSubObject();

	/// <summary>
	/// Creates a D3D12 state subobject that represents the global root signature.
	/// </summary>
	/// <returns>A D3D12_STATE_SUBOBJECT configured to represent the global root signature for a pipeline state.</returns>
	D3D12_STATE_SUBOBJECT createGlobalRootSignatureSubObject();

	/// <summary>
	/// Creates and initializes the ray tracing pipeline state used for GPU ray tracing operations.
	/// </summary>
	void createRayTracingPipelineState();

	/// <summary>
	/// Creates and initializes the shader binding table.
	/// </summary>
	void createShaderBindingTable(const QLabel* frame);

	/// <summary>
	/// Copies the Shader Binding Table (SBT) data for a ray-generation entry into the upload heap.
	/// </summary>
	/// <param name="rayGenID">Pointer identifying the ray-generation entry (or its associated SBT data) to copy into the upload heap.</param>
	void copySBTDataToUploadHeap(void* rayGenID);

	/// <summary>
	/// Copies SBT data to the default heap used by the program.
	/// </summary>
	void copySBTDataToDefaultHeap();

	/// <summary>
	/// Prepares a dispatch rays descriptor using the specified size.
	/// </summary>
	/// <param name="size">The size used to configure the dispatch rays descriptor (passed by const reference).</param>
	void prepareDispatchRaysDesc(const UINT& size, const QLabel* frame);

	/// <summary>
	/// Prepares DirectX for Ray Tracing
	/// </summary>
	void prepareForRayTracing(const QLabel* frame);

	/// <summary>
	/// Prepares DirectX for Rasterization
	/// </summary>
	void prepareForRasterization();
private:
	IDXGIFactory4Ptr dxgiFactory = nullptr; //COM Pointer to the DXGI Factory
	IDXGIAdapter1Ptr adapter = nullptr; //COM Pointer to the used for rendering adapter
	ID3D12Device5Ptr device = nullptr; //COM Pointer to the used for rendering device

	ID3D12CommandQueuePtr commandQueue = nullptr; //COM Pointer to the command queue
	ID3D12CommandAllocatorPtr commandAllocator = nullptr; //COM Pointer to the command allocator
	ID3D12GraphicsCommandList4Ptr graphicsCommandList = nullptr; //COM Pointer to the command list

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

	std::unique_ptr<VertexBuffer> vertexBuffer; //Vertex buffer resource
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
	std::unique_ptr<GPUDefaultHeap> gpuDefaultHeap; //Default heap for vertices on the GPU

	unsigned int frameIdx=0; //Current frame index
	bool triangleDirection; //Direction of movement for the triangle

	std::unique_ptr<OutputTexture> outputTexture; //2D output texture for RT
	ID3D12DescriptorHeapPtr UAVDescHeapHandle; //Descriptor heap handle for Unordered Access View

	ID3D12RootSignaturePtr globalRootSignature; //Global root signature for RT

	ID3DBlobPtr rayGenBlob; //Blob for the ray generation shader
	D3D12_EXPORT_DESC rayGenExportDesc; //Export description for the ray generation shader
	D3D12_DXIL_LIBRARY_DESC rayGenLibDesc; //DXIL library description for the ray generation shader

	ID3DBlobPtr missShaderBlob; //Blob for the miss shader
	D3D12_EXPORT_DESC missShaderExportDesc; //Export description for the miss shader
	D3D12_DXIL_LIBRARY_DESC missShaderLibDesc; //DXIL library description for the miss shader

	D3D12_RAYTRACING_SHADER_CONFIG rayTracingShaderConfig; //Ray tracing shader config
	D3D12_RAYTRACING_PIPELINE_CONFIG rayTracingPipelineConfig; //Ray tracing pipeline config
	D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignatureDesc; //Global root signature description

	ID3D12StateObjectPtr rtStateObject; //COM Pointer to the ray tracing state object

	std::unique_ptr <SBTUploadHeap> sbtUploadHeap; //Shader binding table upload heap
	std::unique_ptr <SBTDefaultHeap> sbtDefaultHeap; //Shader binding table default heap
	D3D12_DISPATCH_RAYS_DESC dispatchRaysDesc = {}; //Dispatch rays descriptor
};