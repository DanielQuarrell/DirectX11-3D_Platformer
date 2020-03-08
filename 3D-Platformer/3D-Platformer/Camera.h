#pragma once
#include "Graphics.h"
#include "Player.h"

class Camera
{
public:
	Camera(Player* _objectToFollow);
	DirectX::XMMATRIX GetMatrix() const;
	void SetMovementTransform(float rMovement);
	void Reset();
private:
	//Positionals
	float radius = 5.0f; //Distance from origin
	float height = 2.5f; //Height above origin
	//Angular
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	//Pointer reference to the player to access its transform matrix
	Player* player = nullptr;
};
