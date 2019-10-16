#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, ID3DBlob* pVertexShaderBytecode);
	~InputLayout();
	void Bind(Graphics& gfx) noexcept override;

protected:
	ID3D11InputLayout* pInputLayout = nullptr;
};