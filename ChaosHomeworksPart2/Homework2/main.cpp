#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <iostream>
#include <string>

int main() {
	IDXGIFactory4* dxgiFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(
		IID_PPV_ARGS(&dxgiFactory)
	);

	if (SUCCEEDED(hr)) {
		IDXGIAdapter1* adapter = nullptr;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
			DXGI_ADAPTER_DESC1 desc;
			hr = adapter->GetDesc1(&desc);
			if (FAILED(hr)) {
				continue;
			}

			std::wcout << desc.Description << "\n";
			std::cout << "Video Memory: " << desc.DedicatedVideoMemory / 1024 / 1024 << " MB\n";

			adapter->Release();
		}

		ID3D12Device* device;
		hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		if (FAILED(hr)) {
			std::cout << "Failed creating D3D12 Device\n";
			return -1;
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ID3D12CommandQueue* commandQueue;
		hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
		if (FAILED(hr)) {
			std::cout << "Failed creating D3D12 Command Queue\n";
			return -1;
		}

		ID3D12CommandAllocator* commandAllocator;
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		if (FAILED(hr)) {
			std::cout << "Failed creating D3D12 Command Allocator\n";
			return -1;
		}

		ID3D12GraphicsCommandList1* graphicsCommandList;
		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&graphicsCommandList));
		if (FAILED(hr)) {
			std::cout << "Failed creating D3D12 Command List\n";
			return -1;
		}

		std::cout << "Success!";
		dxgiFactory->Release();
	}
}