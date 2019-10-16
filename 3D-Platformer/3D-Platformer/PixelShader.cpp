#include "PixelShader.h"

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
{
	//Create pixel shader
	ID3DBlob* pBlob;
	D3DReadFileToBlob(path.c_str(), &pBlob);
	GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	pBlob->Release();
}

PixelShader::~PixelShader()
{
	if (pPixelShader != nullptr)
	{
		pPixelShader->Release();
	}
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader, nullptr, 0u);
}