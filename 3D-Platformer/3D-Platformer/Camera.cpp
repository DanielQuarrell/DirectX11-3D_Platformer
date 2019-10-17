#include "Camera.h"

DirectX::XMMATRIX Camera::GetMatrix() const
{
	const DirectX::XMVECTOR position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0, 0.0f, -radius, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);

	//Second parameter is a location to look at, can pass in an object transform
	//Third parameter is an up transformation

	return DirectX::XMMatrixLookAtLH(
		position, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);
}

void Camera::SetMovementTransform(float rMovement, float thetaMovement)
{
	radius += rMovement;

	if (radius < 1)
	{
		radius = 1;
	}

	theta += thetaMovement;
}

void Camera::Reset()
{
	radius	= 20.0f;
	theta	= 0.0f;
	phi		= 0.0f;
	pitch	= 0.0f;
	yaw		= 0.0f;
	roll	= 0.0f;
}
