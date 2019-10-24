#pragma once
#include "GameObjectBase.h"
#include <string>

class Player : public GameObjectBase<Player>
{
public:
	Player(Graphics& gfx, float _x, float _y, float _z);
	void SetVelocity(float _horizontal, float _verticle, float _z);
	void SetPosition(float _x, float _y, float _z);
	void SetEularX(float angle);
	void SetEularY(float angle);
	void SetEularZ(float angle);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	//Structure for vertex
	struct Vertex
	{
		Vertex() {};
		Vertex(
			float x, float y, float z,
			float u, float v) :
			pos(x, y, z), texCoord(u, v) {}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normals;
	};

	//Speed
	float speed = 4.0f;
	float rotationSpeed = 1.5f;
	//Velocity
	float xVel = 0.0f;
	float yVel = 0.0f;
	float zVel = 0.0f;
	//Position
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;
	//Rotation
	float xRot = 0.0f;
	float yRot = 0.0f;
	float zRot = 0.0f;
	//Model transform
	DirectX::XMFLOAT3X3 modelTransform;

	std::wstring textureName = L"player.png";

	std::vector<Vertex> vertices;
};
