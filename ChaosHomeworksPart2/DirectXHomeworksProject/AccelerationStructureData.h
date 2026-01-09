#pragma once
#include "DXResource.h"
enum class AccelerationStructureDataType
{
    DESTINATION,
	SCRATCH
};
class AccelerationStructureData :
    public DXResource
{
public://Public Functions
    AccelerationStructureData(ID3D12Device* device, const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO* prebuildInfo, const AccelerationStructureDataType type);
	virtual ~AccelerationStructureData() = default;

};

