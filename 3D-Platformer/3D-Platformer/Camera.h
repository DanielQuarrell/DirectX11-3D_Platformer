#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const;
	void SetMovementTransform(float rMovement, float thetaMovement);
	void Reset();
private:
	//Positionals
	float radius = 20.0f; //Distance from origin
	float theta = 0.0f;	
	float phi = 0.0f;
	//Angular
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};
