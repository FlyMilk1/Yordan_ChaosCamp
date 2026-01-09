#pragma once
#include "DXResource.h"
#include "CameraBuffer.h"
class CameraBufferResource :
    public DXResource
{
public:
    CameraBufferResource(ID3D12Device* device);
    virtual ~CameraBufferResource() = default;

    /// <summary>
    /// Moves the camera by the specified offset.
    /// </summary>
    /// <param name="deltaPosition">Offset to apply to the camera's position as a DirectX::XMFLOAT3 (x, y, z). This vector is added to the current camera position.</param>
    void moveCamera(const DirectX::XMFLOAT3& deltaPosition);
    
private:
	CameraCB cameraData = {};
};

