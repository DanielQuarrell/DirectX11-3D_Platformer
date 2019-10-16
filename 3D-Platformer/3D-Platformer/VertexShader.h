#pragma once
#include "Bindable.h"
#include <String>

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	~VertexShader();
	void Bind(Graphics& gfx) noexcept override;
	ID3DBlob* GetBytecode() const noexcept;
protected:
	ID3DBlob* pBytecodeBlob = nullptr;
	ID3D11VertexShader* pVertexShader = nullptr;
};