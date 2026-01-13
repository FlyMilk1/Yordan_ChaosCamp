#include "CameraSceneObject.h"
CameraSceneObject::CameraSceneObject(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const std::string& name)
{
	this->position = position;
	this->rotation = rotation;
	this->type = CAMERA;
	this->name = name;
}
const CameraCB& CameraSceneObject::getCameraBuffer()
{
	calculateCameraRotationVectors();
	cameraBuffer.cameraPosition = position;
	return cameraBuffer;
}

void CameraSceneObject::calculateCameraRotationVectors()
{
	DirectX::XMVECTOR forward = DirectX::XMVectorSet(
		cosf(rotation.z) * sinf(rotation.y),
		sinf(rotation.z),
		cosf(rotation.z) * cosf(rotation.y),
		0.0f
	);

	forward = DirectX::XMVector3Normalize(forward);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, forward));

	up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(forward, right));

	cameraBuffer.cameraForward = forward;
	cameraBuffer.cameraUp = up;
	cameraBuffer.cameraRight = right;
}
