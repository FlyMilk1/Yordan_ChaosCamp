#include "DXRenderer.h"

void DXRenderer::render(const FLOAT* RGBAcolor)
{
	CustomStopwatch preparationStopwatch;
	CustomStopwatch renderingStopwatch;

	preparationStopwatch.start();
	prepareForRendering();
	preparationStopwatch.stop();

	renderingStopwatch.start();
	renderFrame(RGBAcolor, TRUE);
	renderingStopwatch.stop();

	std::cout << "Successful rendering" << std::endl;
	std::cout << "Preparion time: " << preparationStopwatch.getDurationMilli().count() << " ms" << std::endl;
	std::cout << "Rendering time: " << renderingStopwatch.getDurationMilli().count() << " ms" << std::endl;

	cleanUp();
}

void DXRenderer::renderFrame(const FLOAT* RGBAcolor, const bool& writeToFile)
{
	graphicsCommandList->OMSetRenderTargets(1, &CPUDescriptorHandle, FALSE, nullptr);
	graphicsCommandList->ClearRenderTargetView(CPUDescriptorHandle, RGBAcolor, 0, NULL);

	flipBarrier(FALSE);

	graphicsCommandList->ResourceBarrier(1, &barrier);
	graphicsCommandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

	flipBarrier(TRUE);

	graphicsCommandList->ResourceBarrier(1, &barrier);
	graphicsCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { graphicsCommandList };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	commandQueue->Signal(frameFence, FRAME_FENCE_COMPLETION_VALUE);

	waitForGPURenderFrame();
	
	if(writeToFile)	writeImageToFile();

	HRESULT hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));

	hr = graphicsCommandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
}

void DXRenderer::prepareForRendering()
{
	createDevice();
	createCommandsManager();
	RTResource.createRenderTarget(device, &CPUDescriptorHandle);
	placedFootprint = ReadbackResource.createGPUReadBackHeap(device, &RTResource);
	createBarrier();
	createSourceDest();
	createFence();
}

void DXRenderer::createDevice()
{
	HRESULT hr = CreateDXGIFactory1(
		IID_PPV_ARGS(&dxgiFactory)
	);

	assert(SUCCEEDED(hr));
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		//Uses first GPU
		if (SUCCEEDED(hr)) {
			std::wcout << "Using GPU: " << desc.Description << "\n";
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
}

void DXRenderer::createFence()
{
	HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence));
	assert(SUCCEEDED(hr));

	frameEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(frameEventHandle);
}

void DXRenderer::createBarrier()
{
	barrier.Transition.pResource = RTResource.getD3D12Resource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
}
void DXRenderer::flipBarrier(const bool& direction) {
	if (direction) {
		barrier.Transition.StateBefore =  D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}
	else {
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	}
}

void DXRenderer::createSourceDest()
{
	source.pResource = RTResource.getD3D12Resource();
	source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	source.SubresourceIndex = 0;

	destination.pResource = ReadbackResource.getD3D12Resource();
	destination.PlacedFootprint = placedFootprint;
	destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
}

void DXRenderer::waitForGPURenderFrame(){
	if (frameFence->GetCompletedValue() != FRAME_FENCE_COMPLETION_VALUE) {
		HRESULT hr = frameFence->SetEventOnCompletion(FRAME_FENCE_COMPLETION_VALUE, frameEventHandle);
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
			for (int channelIdx = 0; channelIdx < RGBA_COLOR_CHANNELS_COUNT - 1; ++channelIdx) {
				file << static_cast<int>(pixelData[channelIdx]) << " ";
			}
		}
		file << "\n";
	}

	file.close();
	ReadbackResource.getD3D12Resource()->Unmap(0, nullptr);
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

