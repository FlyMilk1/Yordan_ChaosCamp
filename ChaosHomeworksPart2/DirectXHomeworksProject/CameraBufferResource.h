#pragma once
#include "DXResource.h"
#include "CameraBuffer.h"
class CameraBufferResource :
    public DXResource
{
public:
    CameraBufferResource(ID3D12Device* device);
    virtual ~CameraBufferResource() = default;
    
private:
	CameraCB cameraData = {};
};

