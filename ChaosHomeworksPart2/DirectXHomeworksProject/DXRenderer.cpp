#include "DXRenderer.h"
#include "ShaderCompiler.h"
#include "SceneObject.h"
DXRenderer::DXRenderer()
{
#ifdef _DEBUG
#include <iostream>
	ID3D12DebugPtr debugController = nullptr;
	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	if (SUCCEEDED(hr)) {
		debugController->EnableDebugLayer();
		OutputDebugStringA("[D3D12 DEBUG] Debug Layer ENABLED\n");
		debugController->Release();
	}
	else {
		OutputDebugStringA("[D3D12 DEBUG] Debug Layer NOT AVAILABLE\n");
	}
#endif

}

QImage DXRenderer::renderFrame(const FrameData& frameData, const bool& writeToFile)
{
	frameBegin();

	if (isUsingRayTracing) {
		ID3D12DescriptorHeap* descriptorHeaps[] = { rtDescriptorHeap };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		commandList->SetComputeRootSignature(globalRootSignature);

		commandList->SetComputeRootConstantBufferView(1, cameraBuffer->getD3D12Resource()->GetGPUVirtualAddress());

		commandList->SetComputeRootDescriptorTable(0, rtDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		commandList->SetPipelineState1(rtStateObject);

		commandList->DispatchRays(&dispatchRaysDesc);
	}
	else {

		commandList->SetGraphicsRootSignature(rootSignature);
		commandList->SetPipelineState(pipelineState);

		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, vertexBuffer->getVertexBufferViewPointer());


		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);

		D3D12_VERTEX_BUFFER_VIEW vb = *vertexBuffer->getVertexBufferViewPointer();
		commandList->IASetVertexBuffers(0, 1, &vb);

		commandList->SetGraphicsRoot32BitConstant(0, frameIdx, 0);
		commandList->SetGraphicsRoot32BitConstant(1, *reinterpret_cast<const UINT*>(&frameData.offsetX), 1);
		commandList->SetGraphicsRoot32BitConstant(2, *reinterpret_cast<const UINT*>(&frameData.offsetY), 2);

		commandList->DrawInstanced(vertexBuffer->getVerticesCount(), 1, 0, 0);
	}

	
	frameEnd();
	return {};
}

void DXRenderer::prepareForRendering(const QLabel* frame, const bool& useRayTracing)
{
	if (!device) {
		createDevice();
		createCommandsManager();
		vertexBuffer = std::make_unique<VertexBuffer>(device);
		createSwapChain(frame);
		createRTVs();
		/*ReadbackResource = GPUReadbackHeapResource(device, &RTResource);
		placedFootprint = ReadbackResource.getPlacedFootprint();*/
		createFence();
		checkRayTracingSupport();
	}
	
	isUsingRayTracing = useRayTracing;
	createVertexBuffer();
	if (useRayTracing) {
		prepareForRayTracing(frame);
	}
	else {
		prepareForRasterization(frame);
	}
}

void DXRenderer::setBackgroundColor(const float& r, const float& g, const float& b, const float& a)
{
	clearColor[0] = r;
	clearColor[1] = g;
	clearColor[2] = b;
	clearColor[3] = a;
}

void DXRenderer::createDevice()
{

	HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,
		IID_PPV_ARGS(&dxgiFactory)
	);

	assert(SUCCEEDED(hr));
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		//Uses first GPU
		if (SUCCEEDED(hr)) {
			//std::wcout << "Using GPU: " << desc.Description << "\n";
			break;
		}

	}

	assert(adapter);
}

void DXRenderer::createCommandsManager()
{
	const D3D12_COMMAND_LIST_TYPE commandsType = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = commandsType;

	HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr));

	hr = device->CreateCommandAllocator(commandsType, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	hr = device->CreateCommandList(0, commandsType, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

}

void DXRenderer::createFence()
{
	HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence));
	assert(SUCCEEDED(hr));

	frameEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(frameEventHandle);
}

void DXRenderer::setBarrier(const D3D12_RESOURCE_STATES& beforeState, const D3D12_RESOURCE_STATES& afterState)
{
	HRESULT hr = swapChain3->GetBuffer(currentSwapChainBackBufferIndex, IID_PPV_ARGS(&backBuffer));
	assert(SUCCEEDED(hr));

	barrier.Transition.pResource = backBuffer;
	barrier.Transition.StateBefore = beforeState;
	barrier.Transition.StateAfter = afterState;

	commandList->ResourceBarrier(1, &barrier);
}

void DXRenderer::setSourceDest(const UINT& resourceIndex)
{
	source.pResource = RTResource.getRTVResource(resourceIndex);
	source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	source.SubresourceIndex = 0;

	destination.pResource = ReadbackResource.getD3D12Resource();
	destination.PlacedFootprint = placedFootprint;
	destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
}

void DXRenderer::waitForGPURenderFrame(){
	if (frameFence->GetCompletedValue() < renderFrameFenceValue) {
		HRESULT hr = frameFence->SetEventOnCompletion(renderFrameFenceValue, frameEventHandle);
		assert(SUCCEEDED(hr));

		WaitForSingleObject(frameEventHandle, INFINITE);
	}
}

void DXRenderer::writeImageToFile()
{
	void* renderTargetData;
	HRESULT hr = ReadbackResource.getD3D12Resource()->Map(0, nullptr, &renderTargetData);
	assert(SUCCEEDED(hr));

	std::string filename{"output.ppm"};
	std::ofstream file(filename, std::ios::out | std::ios::binary);
	assert(file);

	D3D12_RESOURCE_DESC textureDesc = RTResource.getResourceDescription();
	file << "P3\n" << textureDesc.Width << " " << textureDesc.Height << "\n255\n";

	for (UINT rowIdx = 0; rowIdx < textureDesc.Height; ++rowIdx) {
		UINT rowPitch = placedFootprint.Footprint.RowPitch;
		uint8_t* rowData = reinterpret_cast<uint8_t*>(renderTargetData) + rowIdx * rowPitch;

		for (UINT64 colIdx = 0; colIdx < textureDesc.Width; ++colIdx) {

			uint8_t* pixelData = rowData + colIdx * RGBA_COLOR_CHANNELS_COUNT;
			for (UINT channelIdx = 0; channelIdx < RGBA_COLOR_CHANNELS_COUNT - 1; ++channelIdx) {
				file << static_cast<int>(pixelData[channelIdx]) << " ";
			}
		}
		file << "\n";
	}

	file.close();
	ReadbackResource.getD3D12Resource()->Unmap(0, nullptr);
}

QImage DXRenderer::renderTargetDataToQimage(void* renderTargetData, const UINT64& width, const UINT64& height, const UINT& rowPitch){
    QImage image(width, height, QImage::Format_RGBA8888);

    for (UINT row = 0; row < height; ++row) {
        const uint8_t* src = reinterpret_cast<uint8_t*>(renderTargetData) + row * rowPitch;
        uint8_t* dst = image.scanLine(row);
        memcpy(dst, src, width * 4);
    }

    return image;
}

void DXRenderer::createSwapChain(const QLabel* frame)
{
	if (swapChain3) {
		return;
	}
	IDXGISwapChain1Ptr swapChain1;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = frame->width();
	swapChainDesc.Height = frame->height();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0;


	rtvHandle = HWND(frame->winId());

	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue,
		rtvHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1
	);
	assert(SUCCEEDED(hr));

	hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain3));
	assert(SUCCEEDED(hr));

	
}

void DXRenderer::frameBegin()
{
	waitForGPURenderFrame();
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

	
	currentSwapChainBackBufferIndex = swapChain3->GetCurrentBackBufferIndex();
	setBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	if (isUsingRayTracing) {
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Transition.pResource = outputTexture->getD3D12Resource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		commandList->ResourceBarrier(1, &barrier);
	}
	

	D3D12_CPU_DESCRIPTOR_HANDLE currentRTV = CPUDescriptorHandle;
	currentRTV.ptr += currentSwapChainBackBufferIndex * rtvDescriptorSize;
	commandList->OMSetRenderTargets(1, &currentRTV, FALSE, nullptr);
	commandList->ClearRenderTargetView(currentRTV, clearColor, 0, NULL);
}

void DXRenderer::frameEnd()
{
	if (isUsingRayTracing) {

		D3D12_RESOURCE_BARRIER barriers[2] = {};

		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Transition.pResource = outputTexture->getD3D12Resource();
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Transition.pResource = backBuffer;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		commandList->ResourceBarrier(2, barriers);

		commandList->CopyResource(backBuffer, outputTexture->getD3D12Resource());

		setBarrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
	}
	else {
		setBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}

	commandList->Close();

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);

	
	swapChain3->Present(1, 0);
	//waitForGPURenderFrame();
	++frameIdx;
}

void DXRenderer::createRTVs()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2; 
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	assert(SUCCEEDED(hr));

	CPUDescriptorHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = CPUDescriptorHandle;

	for (UINT i = 0; i < 2; ++i) {
		ID3D12Resource* backBuffer = nullptr;
		swapChain3->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		device->CreateRenderTargetView(backBuffer, nullptr, handle);
		handle.ptr += rtvDescriptorSize;
	}
}

void DXRenderer::createRootSignature()
{
	CD3DX12_ROOT_PARAMETER1 rootParam{};
	rootParam.InitAsConstants(3, 0, 0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(1, &rootParam, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlobPtr signature;
	ID3DBlobPtr error;

	HRESULT hr = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_1,
		&signature,
		&error
	);
	assert(SUCCEEDED(hr));

	hr = device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(hr));
}

void DXRenderer::createPipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	psDesc.pRootSignature = rootSignature;
	psDesc.PS = { g_const_color, _countof(g_const_color) };
	psDesc.VS = { g_const_color_vs, _countof(g_const_color_vs) };
	psDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psDesc.DepthStencilState.DepthEnable = FALSE;
	psDesc.DepthStencilState.StencilEnable = FALSE;
	psDesc.SampleMask = UINT_MAX;
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.SampleDesc.Count = 1;
	psDesc.SampleDesc.Quality = 0;

	HRESULT hr = device->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(hr));

}

void DXRenderer::createViewport(const QLabel* frame)
{
	viewport = D3D12_VIEWPORT{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = frame->width();
	viewport.Height = frame->height();

	scissorRect = D3D12_RECT{};
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = frame->width();
	scissorRect.bottom = frame->height();
}

void DXRenderer::updateSceneVerticesVB(const Scene* scene)
{
	vertexBuffer->addVerticesToBuffer(Scene::getMeshVertices(scene->getSceneObjectsByType(ObjectType::GEOMETRY)));
	vertexBuffer->updateTriangles();

	waitForGPURenderFrame();
	gpuDefaultHeap = std::make_unique<GPUDefaultHeap>(device, vertexBuffer->getVerticesCount());
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

	commandList->CopyBufferRegion(gpuDefaultHeap->getD3D12Resource(),
		0,
		vertexBuffer->getD3D12Resource(),
		0,
		vertexBuffer->getVerticesCount() * sizeof(Vertex));
	D3D12_RESOURCE_BARRIER vbBarrier{};
	vbBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	vbBarrier.Transition.pResource = gpuDefaultHeap->getD3D12Resource();
	vbBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	vbBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	vbBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &vbBarrier);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);

	waitForGPURenderFrame();

	prepareAccelerationStructures();
}

void DXRenderer::createVertexBuffer()
{
	vertexBuffer->addVerticesToBuffer({
	{0.0, 0.5, 0.0},
	{0.5, -0.5, 0.0},
	{-0.5, -0.5, 0.0}
		});
	vertexBuffer->updateTriangles();
	gpuDefaultHeap = std::make_unique<GPUDefaultHeap>(device, vertexBuffer->getVerticesCount());
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

	commandList->CopyBufferRegion(gpuDefaultHeap->getD3D12Resource(), 
		0, 
		vertexBuffer->getD3D12Resource(), 
		0, 
		vertexBuffer->getVerticesCount() * sizeof(Vertex));
	D3D12_RESOURCE_BARRIER vbBarrier{};
	vbBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	vbBarrier.Transition.pResource = gpuDefaultHeap->getD3D12Resource();
	vbBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	vbBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	vbBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &vbBarrier);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);

	waitForGPURenderFrame();
}

void DXRenderer::createOutputTexture(const QLabel* frame)
{
	outputTexture = std::make_unique<OutputTexture>(device, frame->width(), frame->height());

	D3D12_DESCRIPTOR_HEAP_DESC textureDescHeap = {};
	textureDescHeap.NumDescriptors = 1;
	textureDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	textureDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	HRESULT hr = device->CreateDescriptorHeap(&textureDescHeap, IID_PPV_ARGS(&UAVDescHeapHandle));
	assert(SUCCEEDED(hr));

	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(outputTexture->getD3D12Resource(), nullptr, &UAVDesc, UAVDescHeapHandle->GetCPUDescriptorHandleForHeapStart());
}

void DXRenderer::createGlobalRootSignature()
{
	D3D12_DESCRIPTOR_RANGE uavRange = {};
	uavRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	uavRange.NumDescriptors = 1;
	uavRange.BaseShaderRegister = 0;
	uavRange.RegisterSpace = 0;
	uavRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE srvRange = {};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;
	srvRange.BaseShaderRegister = 0;
	srvRange.RegisterSpace = 0;
	srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRanges[2] = { uavRange, srvRange };

	D3D12_ROOT_PARAMETER descriptorTableParam = {};
	descriptorTableParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	descriptorTableParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	descriptorTableParam.DescriptorTable.NumDescriptorRanges = 2;
	descriptorTableParam.DescriptorTable.pDescriptorRanges = descriptorRanges;

	D3D12_ROOT_PARAMETER cameraParam = {};
	cameraParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	D3D12_ROOT_PARAMETER rootParameters[2] = { descriptorTableParam, cameraParam };

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 2;
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ID3DBlobPtr sigBlob;
	ID3DBlobPtr errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(
		&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errorBlob);
	assert(SUCCEEDED(hr));

	hr = device->CreateRootSignature(
		0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(&globalRootSignature)
	);
	assert(SUCCEEDED(hr));
}

D3D12_STATE_SUBOBJECT DXRenderer::createRayGenLibSubObject()
{
	rayGenBlob = ShaderCompiler::compileShaders(L"ray_tracing_shader.hlsl", L"rayGen", L"lib_6_5");

	rayGenExportDesc = D3D12_EXPORT_DESC{};
	rayGenExportDesc.Name = L"rayGen";
	rayGenExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

	rayGenLibDesc.DXILLibrary.pShaderBytecode = rayGenBlob->GetBufferPointer();
	rayGenLibDesc.DXILLibrary.BytecodeLength = rayGenBlob->GetBufferSize();
	rayGenLibDesc.NumExports = 1;
	rayGenLibDesc.pExports = &rayGenExportDesc;

	D3D12_STATE_SUBOBJECT rayGenLibSubObject = {};
	rayGenLibSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	rayGenLibSubObject.pDesc = &rayGenLibDesc;

	return rayGenLibSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createMissShaderLibSubObject()
{
	missShaderBlob = ShaderCompiler::compileShaders(L"ray_tracing_shader.hlsl", L"miss", L"lib_6_5");

	missShaderExportDesc = D3D12_EXPORT_DESC{};
	missShaderExportDesc.Name = L"miss";
	missShaderExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

	missShaderLibDesc.DXILLibrary.pShaderBytecode = missShaderBlob->GetBufferPointer();
	missShaderLibDesc.DXILLibrary.BytecodeLength = missShaderBlob->GetBufferSize();
	missShaderLibDesc.NumExports = 1;
	missShaderLibDesc.pExports = &missShaderExportDesc;

	D3D12_STATE_SUBOBJECT missShaderLibSubObject = {};
	missShaderLibSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	missShaderLibSubObject.pDesc = &missShaderLibDesc;

	return missShaderLibSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createRayTracingShaderConfigSubObject()
{
	rayTracingShaderConfig = D3D12_RAYTRACING_SHADER_CONFIG{};
	rayTracingShaderConfig.MaxPayloadSizeInBytes = 4 * 4; // RGBA
	rayTracingShaderConfig.MaxAttributeSizeInBytes = 8;

	D3D12_STATE_SUBOBJECT rayTracingShaderConfigSubObject = {};
	rayTracingShaderConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
	rayTracingShaderConfigSubObject.pDesc = &rayTracingShaderConfig;

	return rayTracingShaderConfigSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createPipelineConfigSubObject()
{
	rayTracingPipelineConfig = D3D12_RAYTRACING_PIPELINE_CONFIG{};
	rayTracingPipelineConfig.MaxTraceRecursionDepth = 1;

	D3D12_STATE_SUBOBJECT pipelineConfigSubObject = {};
	pipelineConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	pipelineConfigSubObject.pDesc = &rayTracingPipelineConfig;

	return pipelineConfigSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createGlobalRootSignatureSubObject()
{
	globalRootSignatureDesc = D3D12_GLOBAL_ROOT_SIGNATURE{globalRootSignature};

	D3D12_STATE_SUBOBJECT globalRootSignatureSubObject = {};
	globalRootSignatureSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
	globalRootSignatureSubObject.pDesc = &globalRootSignatureDesc;

	return globalRootSignatureSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createCloserHitShaderSubObject()
{
	closestHitShaderBlob = ShaderCompiler::compileShaders(L"ray_tracing_shader.hlsl", L"closestHit", L"lib_6_5");

	closestHitShaderExportDesc = D3D12_EXPORT_DESC{};
	closestHitShaderExportDesc.Name = L"closestHit";
	closestHitShaderExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

	closestHitShaderLibDesc.DXILLibrary.pShaderBytecode = closestHitShaderBlob->GetBufferPointer();
	closestHitShaderLibDesc.DXILLibrary.BytecodeLength = closestHitShaderBlob->GetBufferSize();
	closestHitShaderLibDesc.NumExports = 1;
	closestHitShaderLibDesc.pExports = &closestHitShaderExportDesc;

	D3D12_STATE_SUBOBJECT closestHitShaderLibSubObject = {};
	closestHitShaderLibSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	closestHitShaderLibSubObject.pDesc = &closestHitShaderLibDesc;

	return closestHitShaderLibSubObject;
}

D3D12_STATE_SUBOBJECT DXRenderer::createHitGroupSubObject()
{

	hitGroupDesc.ClosestHitShaderImport = L"closestHit";
	hitGroupDesc.HitGroupExport = L"HitGroup";
	hitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
	D3D12_STATE_SUBOBJECT hitGroupSubObject = {};
	hitGroupSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
	hitGroupSubObject.pDesc = &hitGroupDesc;
	return hitGroupSubObject;
}

void DXRenderer::createRayTracingPipelineState()
{

	D3D12_STATE_SUBOBJECT rayGenLibSubObject = createRayGenLibSubObject();
	D3D12_STATE_SUBOBJECT closestHitShaderLibSubObject = createCloserHitShaderSubObject();
	D3D12_STATE_SUBOBJECT missShaderLibSubObject = createMissShaderLibSubObject();
	D3D12_STATE_SUBOBJECT rayTracingShaderConfigSubObject = createRayTracingShaderConfigSubObject();
	D3D12_STATE_SUBOBJECT pipelineConfigSubObject = createPipelineConfigSubObject();
	D3D12_STATE_SUBOBJECT globalRootSignatureSubObject = createGlobalRootSignatureSubObject();
	D3D12_STATE_SUBOBJECT hitGroupSubObject = createHitGroupSubObject();

	std::vector<D3D12_STATE_SUBOBJECT> subObjects = {
		rayGenLibSubObject,
		missShaderLibSubObject,
		closestHitShaderLibSubObject,
		hitGroupSubObject,
		rayTracingShaderConfigSubObject,
		globalRootSignatureSubObject,
		pipelineConfigSubObject
	};

	D3D12_STATE_OBJECT_DESC rtpsoDesc = {};
	rtpsoDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	rtpsoDesc.NumSubobjects = static_cast<UINT>(subObjects.size());
	rtpsoDesc.pSubobjects = subObjects.data();

	HRESULT hr = device->CreateStateObject(&rtpsoDesc, IID_PPV_ARGS(&rtStateObject));
	if (FAILED(hr))
	{
		std::cerr << "CreateStateObject failed, HRESULT = 0x" << std::hex << hr << std::endl;
		assert(false);
	}

}

void DXRenderer::createShaderBindingTable(const QLabel* frame)
{
	HRESULT hr = rtStateObject->QueryInterface(IID_PPV_ARGS(&rtStateObjectProps));

	void* rayGenID = rtStateObjectProps->GetShaderIdentifier(L"rayGen");
	void* missID = rtStateObjectProps->GetShaderIdentifier(L"miss");
	void* hitGroupID = rtStateObjectProps->GetShaderIdentifier(L"HitGroup");

	const UINT shaderIDSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	const UINT recordSize = alignedSize(shaderIDSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

	UINT rayGenOffset = 0;
	UINT missOffset = alignedSize(rayGenOffset + recordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
	UINT hitGroupOffset = alignedSize(missOffset + recordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

	const UINT sbtSize = alignedSize(recordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT) * 3;

	sbtUploadHeap = std::make_unique<SBTUploadHeap>(device, sbtSize);
	sbtDefaultHeap = std::make_unique<SBTDefaultHeap>(device, sbtSize);
	copySBTDataToUploadHeap(rayGenOffset, missOffset, hitGroupOffset, rayGenID, missID, hitGroupID);
	copySBTDataToDefaultHeap();
	prepareDispatchRaysDesc(recordSize, rayGenOffset, missOffset, hitGroupOffset, frame);
}

void DXRenderer::copySBTDataToUploadHeap(const UINT rayGenOffset, const UINT missOffset, const UINT hitGroupOffset, void* rayGenID, void* missID, void* hitGroupID)
{
	uint8_t* pData = nullptr;
	sbtUploadHeap->getD3D12Resource()->Map(0, nullptr, (void**)&pData);

	memcpy(pData + rayGenOffset, rayGenID, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	memcpy(pData + missOffset, missID, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	memcpy(pData + hitGroupOffset, hitGroupID, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	
	sbtUploadHeap->getD3D12Resource()->Unmap(0, nullptr);
}

void DXRenderer::copySBTDataToDefaultHeap()
{
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
	commandList->CopyResource(sbtDefaultHeap->getD3D12Resource(), sbtUploadHeap->getD3D12Resource());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource = sbtDefaultHeap->getD3D12Resource();

	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);
	waitForGPURenderFrame();
}

void DXRenderer::prepareDispatchRaysDesc(const UINT recordSize, const UINT rayGenOffset, const UINT missOffset, const UINT hitGroupOffset, const QLabel* frame)
{
	UINT64 baseAddress = sbtDefaultHeap->getD3D12Resource()->GetGPUVirtualAddress();
	dispatchRaysDesc.Width = frame->width();
	dispatchRaysDesc.Height = frame->height();
	dispatchRaysDesc.Depth = 1;

	dispatchRaysDesc.RayGenerationShaderRecord.StartAddress = baseAddress + rayGenOffset;
	dispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes = recordSize;

	dispatchRaysDesc.MissShaderTable.StartAddress = baseAddress + missOffset;
	dispatchRaysDesc.MissShaderTable.SizeInBytes = recordSize;
	dispatchRaysDesc.MissShaderTable.StrideInBytes = recordSize;

	dispatchRaysDesc.HitGroupTable.StartAddress = baseAddress + hitGroupOffset;
	dispatchRaysDesc.HitGroupTable.SizeInBytes = recordSize;
	dispatchRaysDesc.HitGroupTable.StrideInBytes = recordSize;

	dispatchRaysDesc.CallableShaderTable = {};
}

void DXRenderer::prepareForRayTracing(const QLabel* frame)
{
	intializeSceneVariables();
	createGlobalRootSignature();
	createRayTracingPipelineState();
	createOutputTexture(frame);
	prepareAccelerationStructures();
	createShaderBindingTable(frame);
}

void DXRenderer::prepareForRasterization(const QLabel* frame)
{
	createRootSignature();
	createPipelineState();
	createViewport(frame);

}

void DXRenderer::describeTriangles()
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC trianglesDesc = {};
	trianglesDesc.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	trianglesDesc.VertexBuffer.StartAddress = gpuDefaultHeap->getD3D12Resource()->GetGPUVirtualAddress();
	trianglesDesc.VertexBuffer.StrideInBytes = sizeof(Vertex);
	trianglesDesc.VertexCount = vertexBuffer->getVerticesCount();

	
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geometryDesc.Triangles = trianglesDesc;
}

void DXRenderer::prepareAccelerationStructures()
{
	describeTriangles();
	buildBLAS();
	buildTLAS();
	createDescriptorsForRTAccelerationStructures();
}

void DXRenderer::buildBLAS()
{
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;
	bottomLevelInputs.NumDescs = 1;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelASInfo = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelASInfo);

	BLASdestAccelerationStructureData = std::make_unique<AccelerationStructureData>(device, &bottomLevelASInfo, AccelerationStructureDataType::DESTINATION);
	BLASscratchAccelerationStructureData = std::make_unique<AccelerationStructureData>(device, &bottomLevelASInfo, AccelerationStructureDataType::SCRATCH);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	bottomLevelBuildDesc.Inputs = bottomLevelInputs;
	bottomLevelBuildDesc.DestAccelerationStructureData = BLASdestAccelerationStructureData->getD3D12Resource()->GetGPUVirtualAddress();
	bottomLevelBuildDesc.ScratchAccelerationStructureData = BLASscratchAccelerationStructureData->getD3D12Resource()->GetGPUVirtualAddress();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.UAV.pResource = nullptr;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	commandList->ResourceBarrier(1, &barrier);

	commandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);
	waitForGPURenderFrame();
}

void DXRenderer::buildTLAS()
{
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
	instanceDesc.AccelerationStructure =
		BLASdestAccelerationStructureData
		->getD3D12Resource()
		->GetGPUVirtualAddress();
	// Explicit identity transform
	instanceDesc.Transform[0][0] = 1.0f;
	instanceDesc.Transform[1][1] = 1.0f;
	instanceDesc.Transform[2][2] = 1.0f;

	instanceDesc.InstanceID = 0;
	instanceDesc.InstanceMask = 0xFF;
	instanceDesc.InstanceContributionToHitGroupIndex = 0;
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&instanceDescBuffer)
	);

	void* pData;
	instanceDescBuffer->Map(0, nullptr, &pData);
	memcpy(pData, &instanceDesc, sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
	instanceDescBuffer->Unmap(0, nullptr);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	topLevelInputs.InstanceDescs = instanceDescBuffer->GetGPUVirtualAddress();
	topLevelInputs.NumDescs = 1;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelASInfo = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelASInfo);

	TLASdestAccelerationStructureData = std::make_unique<AccelerationStructureData>(device, &topLevelASInfo, AccelerationStructureDataType::DESTINATION);
	TLASscratchAccelerationStructureData = std::make_unique<AccelerationStructureData>(device, &topLevelASInfo, AccelerationStructureDataType::SCRATCH);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	topLevelBuildDesc.Inputs = topLevelInputs;
	topLevelBuildDesc.DestAccelerationStructureData = TLASdestAccelerationStructureData->getD3D12Resource()->GetGPUVirtualAddress();
	topLevelBuildDesc.ScratchAccelerationStructureData = TLASscratchAccelerationStructureData->getD3D12Resource()->GetGPUVirtualAddress();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.UAV.pResource = nullptr;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	commandList->ResourceBarrier(1, &barrier);;

	commandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);



	hr = commandList->Close();
	assert(SUCCEEDED(hr));
	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);
	waitForGPURenderFrame();
}

void DXRenderer::createDescriptorsForRTAccelerationStructures()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 2;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	HRESULT hr = device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(&rtDescriptorHeap)
	);
	assert(SUCCEEDED(hr));

	UINT incrementSize =
		device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle =
		rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// u0 — output texture
	D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {};
	uav.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	device->CreateUnorderedAccessView(
		outputTexture->getD3D12Resource(),
		nullptr,
		&uav,
		cpuDescHandle
	);

	// t0 — TLAS
	cpuDescHandle.ptr += incrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.ViewDimension =
		D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srv.RaytracingAccelerationStructure.Location =
		TLASdestAccelerationStructureData
		->getD3D12Resource()
		->GetGPUVirtualAddress();
	srv.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	device->CreateShaderResourceView(nullptr, &srv, cpuDescHandle);
}

void DXRenderer::intializeSceneVariables()
{
	cameraBuffer = std::make_unique<CameraBufferResource>(device);
}

void DXRenderer::updateCameraBuffer(const CameraCB& cameraBuffer)
{
	this->cameraBuffer->updateCameraCB(cameraBuffer);
}

void DXRenderer::getFrameColor(int i, float out[3]) {
	// Use sine waves to smoothly cycle R, G, B over frames
	float speed = 0.02f; // smaller = slower cycling
	out[0] = 0.5f + 0.5f * sinf(i * speed + 0.0f);       // Red
	out[1] = 0.5f + 0.5f * sinf(i * speed + 2.094f);     // Green (120° phase)
	out[2] = 0.5f + 0.5f * sinf(i * speed + 4.188f);     // Blue (240° phase)
}
void DXRenderer::cleanUp()
{
	if (adapter) adapter->Release();
	if (dxgiFactory) dxgiFactory->Release();
	if (device) device->Release();
	if (commandList) commandList->Release();
	if (commandAllocator) commandAllocator->Release();
	if (commandQueue) commandQueue->Release();
	if (frameFence) frameFence->Release();
	if (frameEventHandle) CloseHandle(frameEventHandle);
}
void DXRenderer::checkRayTracingSupport()
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
	HRESULT hr = device->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS5,
		&options5,
		sizeof(options5));

	if (FAILED(hr) || options5.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		std::cerr << "Ray tracing not supported on this device!" << std::endl;
		assert(false);
	}
}

static inline UINT alignedSize(UINT size, UINT alignBytes) {
	return alignBytes * (size / alignBytes + (size % alignBytes ? 1 : 0));
}

