#include "AccelerationStructureData.h"
#include <stdexcept>
AccelerationStructureData::AccelerationStructureData(
    ID3D12Device* device,
    const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO* prebuildInfo,
    const AccelerationStructureDataType type)
{
    heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (type == AccelerationStructureDataType::DESTINATION)
    {
        resourceDesc.Width = prebuildInfo->ResultDataMaxSizeInBytes;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
            nullptr,
            IID_PPV_ARGS(&d3d12Resource));

        if (FAILED(hr) || !d3d12Resource)
        {
            throw std::runtime_error("Failed to create BLAS GPU resource.");
        }
    }
    else // scratch buffer
    {
        resourceDesc.Width = prebuildInfo->ScratchDataSizeInBytes;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            nullptr,
            IID_PPV_ARGS(&d3d12Resource));

        if (FAILED(hr) || !d3d12Resource)
        {
            throw std::runtime_error("Failed to create BLAS GPU resource.");
        }
    }

   
}

