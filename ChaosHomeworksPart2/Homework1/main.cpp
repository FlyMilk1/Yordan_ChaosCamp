#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

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
		


		dxgiFactory->Release();
	}
}