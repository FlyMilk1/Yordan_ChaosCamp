#pragma once
#include "SceneObject.h"
#include "CameraBuffer.h"
class CameraSceneObject :
    public SceneObject
{
public:
	CameraSceneObject(const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0), const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0), const std::string& name = "Unnamed");
	const CameraCB& getCameraBuffer();
private://private functions
	void calculateCameraRotationVectors();
private://private variables
	CameraCB cameraBuffer = {};// Camera buffer data

};

