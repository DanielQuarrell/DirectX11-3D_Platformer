#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class GameObject
{
	template<class T>
	friend class GameObjectBase;
public:
	GameObject() = default;
	GameObject(const GameObject&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	virtual ~GameObject() = default;

protected:
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf);
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};