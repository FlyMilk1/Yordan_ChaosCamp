#pragma once
#include <DirectXMath.h>
struct CameraCB
{
    DirectX::XMFLOAT3 cameraPosition;
    DirectX::XMFLOAT3 cameraRotation;
    float padding;
};
