#pragma once
#include "Bindable.h"
#include "WICTextureLoader.h"
#include <string>

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const std::wstring& textureName);
	~Texture();
	void Bind(Graphics& gfx) noexcept override;
protected:
	ID3D11ShaderResourceView* pTextureView = nullptr;
	ID3D11SamplerState* pSamplerLinear = nullptr;
};