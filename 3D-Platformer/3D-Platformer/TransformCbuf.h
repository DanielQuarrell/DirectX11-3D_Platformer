#pragma once
#include "ConstantBuffers.h"
#include "GameObject.h"
#include <DirectXMath.h>

class TransformCbuf : public Bindable
{
public:
	TransformCbuf(Graphics& gfx, const GameObject& parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const GameObject& parent;
};