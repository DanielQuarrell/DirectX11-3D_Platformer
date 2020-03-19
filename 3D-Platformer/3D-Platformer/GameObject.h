#pragma once
#include "Graphics.h"

class Bindable;

class GameObject
{
	template<class T>
	friend class GameObjectBase;
public:
	GameObject() = default;
	GameObject(const GameObject&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	DirectX::XMVECTOR GetPosition();
	DirectX::XMVECTOR GetBBMinVertex();
	DirectX::XMVECTOR GetBBMaxVertex();
	DirectX::XMVECTOR GetCenterVertex();
	void SetPosition(float _x, float _y, float _z);
	void SetVisibility(bool _isVisible);
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	virtual ~GameObject() = default;

protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf);
	void CreateBoundingBox(std::vector<DirectX::XMFLOAT3>& verticesPositions);
	void CalculateAABB(DirectX::XMMATRIX transformMatrix);

	//Model transform
	DirectX::XMFLOAT3X3 modelTransform;

	//Position
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;
	//Rotation
	float xRot = 0.0f;
	float yRot = 0.0f;
	float zRot = 0.0f;
	//Scale
	float xScale = 1.0f;
	float yScale = 1.0f;
	float zScale = 1.0f;

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;

	DirectX::XMVECTOR bbMinVertex;
	DirectX::XMVECTOR bbMaxVertex;
	DirectX::XMVECTOR centerVertex;

	float bbWidth;
	float bbHeight;
	float bbDepth;

	bool isVisible = true;

	std::vector<DirectX::XMFLOAT3> bbVertices;
};