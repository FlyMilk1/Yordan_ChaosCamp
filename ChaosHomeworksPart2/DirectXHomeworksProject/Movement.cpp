#include "Movement.h"

Movement::Movement(DXRenderer* dxRenderer)
{
	renderer = std::unique_ptr<DXRenderer>(dxRenderer);
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

	// Update the camera buffer with the new position deltas
	renderer->updateCameraBuffer(x, y, z);
}
