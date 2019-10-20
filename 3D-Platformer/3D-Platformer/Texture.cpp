#include "Texture.h"

Texture::Texture(Graphics& gfx, const std::wstring& textureName)
{
	DirectX::CreateWICTextureFromFile(GetDevice(gfx), textureName.c_str(), nullptr, &pTextureView);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice(gfx)->CreateSamplerState(&sampDesc, &pSamplerLinear);
}

Texture::~Texture()
{
	if (pTextureView != nullptr)
	{
		pTextureView->Release();
	}

	if (pSamplerLinear != nullptr)
	{
		pSamplerLinear->Release();
	}
}

void Texture::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(0u, 1u, &pTextureView);
	GetContext(gfx)->PSSetSamplers(0u, 1u, &pSamplerLinear);
}
