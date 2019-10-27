#pragma once
#include "GameObjectBase.h"
#include <string>

class Player : public GameObjectBase<Player>
{
public:
	Player(Graphics& gfx, float _x, float _y, float _z);
	void SetPlayerInput(float _horizontal, float _verticle);
	void MultiplyVelocity(float multiplier);
	void SetVelocity(float _xVel, float _yVel, float _zVel);
	void SetSpawnPosition(float _x, float _y, float _z);
	void SetPosition(float _x, float _y, float _z);
	void MovePosition(float _x, float _y, float _z);
	void SetRotationX(float angle);
	void SetRotationY(float angle);
	void SetRotationZ(float angle);
	void ApplyGravity(float dt);
	void Jump();
	void SetGrounded(bool _grounded);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	DirectX::XMVECTOR GetVelocity();
	DirectX::XMVECTOR GetPosition();

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

	//Physics
	bool grounded = false;
	float gravity = 9.8f;
	float speed = 4.0f;
	float rotationSpeed = 1.5f;

	//SpawnPosition
	float spawnX, spawnY, spawnZ;

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
