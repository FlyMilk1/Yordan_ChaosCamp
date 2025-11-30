#include "DXResource.h"

D3D12_RESOURCE_DESC DXResource::getResourceDescription() const
{
	return resourceDesc;
}

ID3D12Resource* DXResource::getD3D12Resource() const
{
	return d3d12Resource;
}

void DXResource::cleanUpResource()
{
	if (d3d12Resource) d3d12Resource->Release();
	if (descriptorHandle) descriptorHandle->Release();
}
