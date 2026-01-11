#pragma once
#include <DirectXMath.h>
struct CameraCB
{
    DirectX::XMFLOAT3 cameraPosition;
    DirectX::XMVECTOR cameraForward;
	DirectX::XMVECTOR cameraUp;
	DirectX::XMVECTOR cameraRight;
};
