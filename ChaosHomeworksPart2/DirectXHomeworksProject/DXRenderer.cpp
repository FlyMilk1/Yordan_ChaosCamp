#include "DXRenderer.h"

DXRenderer::DXRenderer()
{
#ifdef _DEBUG
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

void DXRenderer::render(const FLOAT* RGBAcolor)
{
	//CustomStopwatch preparationStopwatch;
	//CustomStopwatch renderingStopwatch;

	//preparationStopwatch.start();
	//prepareForRendering();
	//preparationStopwatch.stop();

	//renderingStopwatch.start();
	//renderFrame(RGBAcolor, TRUE);
	//renderingStopwatch.stop();

	//std::cout << "Successful rendering" << std::endl;
	//std::cout << "Preparion time: " << preparationStopwatch.getDurationMilli().count() << " ms" << std::endl;
	//std::cout << "Rendering time: " << renderingStopwatch.getDurationMilli().count() << " ms" << std::endl;

	////cleanUp();
}

QImage DXRenderer::renderFrame(const FLOAT* RGBAcolor, const bool& writeToFile)
{
	frameBegin();

	currentSwapChainBackBufferIndex = swapChain3->GetCurrentBackBufferIndex();
	setBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE currentRTV = CPUDescriptorHandle;
	currentRTV.ptr += currentSwapChainBackBufferIndex * rtvDescriptorSize;
	graphicsCommandList->OMSetRenderTargets(1, &currentRTV, FALSE, nullptr);
	graphicsCommandList->ClearRenderTargetView(currentRTV, RGBAcolor, 0, NULL);

	//setSourceDest(currentSwapChainBackBufferIndex);
	//graphicsCommandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

	setBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	graphicsCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { graphicsCommandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	++renderFrameFenceValue;
	commandQueue->Signal(frameFence, renderFrameFenceValue);

	waitForGPURenderFrame();
	swapChain3->Present(1, 0);
	//if(writeToFile)	writeImageToFile();
	//else {
	//	/*void* renderTargetData;
	//	HRESULT hr = ReadbackResource.getD3D12Resource()->Map(0, nullptr, &renderTargetData);
	//	assert(SUCCEEDED(hr));

	//	D3D12_RESOURCE_DESC textureDesc = RTResource.getResourceDescription();

	//	return DXRenderer::renderTargetDataToQimage(renderTargetData, textureDesc.Width, textureDesc.Height, placedFootprint.Footprint.RowPitch);*/
	//	
	//}

	return {};
}

void DXRenderer::prepareForRendering(const QLabel* frame)
{
	createDevice();
	createCommandsManager();
	createSwapChain(frame);
	createRTVs(frame);
	/*ReadbackResource = GPUReadbackHeapResource(device, &RTResource);
	placedFootprint = ReadbackResource.getPlacedFootprint();*/
	createFence();
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

	hr = device->CreateCommandList(0, commandsType, commandAllocator, nullptr, IID_PPV_ARGS(&graphicsCommandList));
	assert(SUCCEEDED(hr));

	hr = graphicsCommandList->Close();
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

	graphicsCommandList->ResourceBarrier(1, &barrier);
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
	IDXGISwapChain1Ptr swapChain1;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = frame->width();
	swapChainDesc.Height = frame->height();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
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
	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = graphicsCommandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
}

void DXRenderer::createRTVs(const QLabel* frame)
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


void DXRenderer::cleanUp()
{
	if (adapter) adapter->Release();
	if (dxgiFactory) dxgiFactory->Release();
	if (device) device->Release();
	if (graphicsCommandList) graphicsCommandList->Release();
	if (commandAllocator) commandAllocator->Release();
	if (commandQueue) commandQueue->Release();
	if (frameFence) frameFence->Release();
	if (frameEventHandle) CloseHandle(frameEventHandle);
}

