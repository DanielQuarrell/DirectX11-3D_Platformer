#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class GameObject
{
public:
	GameObject() = default;
	GameObject(const GameObject&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;

	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;

	virtual ~GameObject() = default;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};