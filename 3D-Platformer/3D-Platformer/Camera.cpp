#include "Camera.h"

Camera::Camera(Player* _player)
{
	player = _player;
}

DirectX::XMMATRIX Camera::GetMatrix() const
{
	const DirectX::XMVECTOR position = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0, height, -radius, 0.0f),
		player->GetTransformXM()
	);

	const DirectX::XMVECTOR focusPosition = DirectX::XMVector3Transform(DirectX::XMVectorZero(), player->GetTransformXM());

	//Second parameter is a location to look at, can pass in an object transform
	//Third parameter is an up transformation

	return DirectX::XMMatrixLookAtLH(
		position, focusPosition,
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);
}

void Camera::SetMovementTransform(float rMovement)
{
	radius += rMovement;

	if (radius < 2)
	{
		radius = 2;
	}
	if (radius > 7)
	{
		radius = 7;
	}
}

void Camera::Reset()
{
	radius	= 20.0f;
	pitch	= 0.0f;
	yaw		= 0.0f;
	roll	= 0.0f;
}
