#include "DXRenderer.h"

void DXRenderer::render()
{
}

void DXRenderer::prepareForRendering()
{
	createDevice();
	createCommandsManager();
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
