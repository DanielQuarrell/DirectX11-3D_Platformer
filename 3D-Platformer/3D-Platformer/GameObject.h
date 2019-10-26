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
	DirectX::XMVECTOR GetBBMinVertex();
	DirectX::XMVECTOR GetBBMaxVertex();
	DirectX::XMVECTOR GetCenterVertex();
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	virtual ~GameObject() = default;

protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf);
	void CreateBoundingBox(std::vector<DirectX::XMFLOAT3>& verticesPositions);
	void CalculateAABB(DirectX::XMMATRIX transformMatrix);


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

	std::vector<DirectX::XMFLOAT3> bbVertices;
	std::vector<unsigned short> bbIndices;
};