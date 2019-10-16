#include "InputLayout.h"

InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, ID3DBlob* pVertexShaderBytecode)
{
	GetDevice(gfx)->CreateInputLayout(
		inputElementDesc.data(), (UINT)inputElementDesc.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	);
}

InputLayout::~InputLayout()
{
	if (pInputLayout != nullptr)
	{
		pInputLayout->Release();
	}
}

void InputLayout::Bind(Graphics& gfx) noexcept
{
	//Bind vertex layout
	GetContext(gfx)->IASetInputLayout(pInputLayout);
}
