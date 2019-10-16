#include "VertexShader.h"

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
	//Create vertex shader
	D3DReadFileToBlob(path.c_str(), &pBytecodeBlob);
	GetDevice(gfx)->CreateVertexShader(pBytecodeBlob->GetBufferPointer(), pBytecodeBlob->GetBufferSize(), nullptr, &pVertexShader);
}

VertexShader::~VertexShader()
{
	if (pBytecodeBlob != nullptr)
	{
		pBytecodeBlob->Release();
	}
	if (pVertexShader != nullptr)
	{
		pVertexShader->Release();
	}
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	//Bind vertex shader
	GetContext(gfx)->VSSetShader(pVertexShader, nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob;
}
