#pragma once
#include "GameObjectBase.h"
#include <fstream>
#include <sstream>
#include <string>

class CustomObj : public GameObjectBase<CustomObj>
{
public:
	CustomObj(Graphics& gfx, std::wstring _modelName, float _x, float _y, float _z, float _yRot, float _scaleX, float _scaleY, float _scaleZ, bool _hasTexture, bool _hasLighting);
	void LoadObjModel(std::wstring filename);
	void SetPosition(float x, float y, float z);
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
		DirectX::XMFLOAT3 normal;
	};

	struct SurfaceMaterial
	{
		std::wstring matName;
		int numOfFaces;
	};

	//Material structure
	struct Material
	{
		std::wstring matName;
		DirectX::XMFLOAT4 difColor;
		bool transparent;
	};

	std::vector<Material> materials;
	std::vector<SurfaceMaterial> surfaceMaterials;

	//Position
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;

	//Scale
	float xScale = 1.0f;
	float yScale = 1.0f;
	float zScale = 1.0f;

	//Rotation
	float xRot = 0.0f;
	float yRot = 0.0f;
	float zRot = 0.0f;

	//Model transform
	DirectX::XMFLOAT3X3 modelTransform;

	bool hasNormals;
	bool hasTexture;
	float totalFaces;

	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;

	std::wstring modelName = L"";
	std::wstring textureName = L"";
};
