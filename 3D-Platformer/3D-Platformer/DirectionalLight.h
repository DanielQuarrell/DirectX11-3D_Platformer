#pragma once
#include "ConstantBuffers.h"
#include "GameObject.h"

class DirectionalLight : public Bindable
{
private:
	struct DirectionalLightCBuf
	{
		DirectX::XMFLOAT3 direction;
		//DirectX::XMFLOAT4 ambient;
		//DirectX::XMFLOAT4 diffuse;
	};
public:
	DirectionalLight(Graphics& gfx, const GameObject& parent);
	void Reset() noexcept;
	void Bind(Graphics& gfx) noexcept override;
private:
	DirectionalLightCBuf cbData;
	static std::unique_ptr<PixelConstantBuffer<DirectionalLightCBuf>> pCbuf;
	const GameObject& parent;
};