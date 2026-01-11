#pragma once
#include "SceneObject.h"
#include "CameraBuffer.h"
class CameraSceneObject :
    public SceneObject
{
public:
	const CameraCB& getCameraBuffer();
private://private functions
	void calculateCameraRotationVectors();
private://private variables
	CameraCB cameraBuffer = {};// Camera buffer data

};

