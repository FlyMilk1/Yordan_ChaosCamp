#include "Movement.h"
#include "CameraSceneObject.h"
Movement::Movement(DXRenderer* dxRenderer, SceneObject* sceneObject)
{
	renderer = std::unique_ptr<DXRenderer>(dxRenderer);
	object = std::unique_ptr<SceneObject>(sceneObject);
}

void Movement::updatePosition(const MovementInput& input)
{
	float x = 0.0f, y = 0.0f, z = 0.0f;//Summation of movement in each axis
	if (input.moveForward)
		z += speed;
	if (input.moveBackward)
		z -= speed;
	if (input.moveLeft)
		x -= speed;
	if (input.moveRight)
		x += speed;
	if (input.moveUp)
		y += speed;
	if (input.moveDown)
		y -= speed;

	object->setPosition(DirectX::XMFLOAT3(
		object->getPosition().x + x,
		object->getPosition().y + y,
		object->getPosition().z + z
	));
	// Update the camera buffer with the new position deltas
	renderer->updateCameraBuffer(static_cast<CameraSceneObject*>(object.get())->getCameraBuffer());
}
