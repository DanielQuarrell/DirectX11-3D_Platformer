#pragma once
#include "GameObjectBase.h"
#include <string>

class Bridge : public GameObjectBase<Bridge>
{
public:
	Bridge(Graphics& gfx, std::wstring _textureName, float _x, float _y, float _z);
	void SetPosition(float x, float y, float z);
	void RaiseBridge(float _speed);
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

	//Position
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;

	float speed = 1.0f;
	float yTarget = 0.0f;
	float bridgeHeight = 0.0f;

	//Rotation
	float xRot = 0.0f;
	float yRot = 0.0f;
	float zRot = 0.0f;
	//Model transform
	DirectX::XMFLOAT3X3 modelTransform;

	std::vector<Vertex> vertices;

	std::wstring textureName = L"";
};

