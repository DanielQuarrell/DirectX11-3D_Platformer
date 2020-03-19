#pragma once
#include "GameObjectBase.h"
#include <string>

class TexturedBox : public GameObjectBase<TexturedBox>
{
public:
	TexturedBox(Graphics& gfx, std::wstring _textureName, float _x, float _y, float _z);
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

	std::vector<Vertex> vertices;

	std::wstring textureName = L"";
};
