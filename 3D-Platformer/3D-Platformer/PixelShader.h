#pragma once
#include "Bindable.h"
#include <string>

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& gfx, const std::wstring& path);
	~PixelShader();
	void Bind(Graphics& gfx) noexcept override;
protected:
	ID3D11PixelShader* pPixelShader = nullptr;
};