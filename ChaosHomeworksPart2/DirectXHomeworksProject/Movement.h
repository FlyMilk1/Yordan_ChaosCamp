#pragma once
#include "DXRenderer.h"
#include "MovementInput.h"
class Movement
{
public://public functions
	Movement(DXRenderer* dxRenderer);
	virtual ~Movement() = default;

	/// <summary>
	/// Updates the current position using the provided movement input.
	/// </summary>
	/// <param name="input">Movement input data used to update position.</param>
	void updatePosition(const MovementInput& input);
private://private variables
	std::unique_ptr<DXRenderer> renderer;//Pointer to DirectX renderer
	float speed = 0.05f;//Movement speed
};

