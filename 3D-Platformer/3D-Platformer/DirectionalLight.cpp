#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Graphics& gfx, const GameObject& parent) : parent(parent)
{
	if (!pCbuf)
	{
		pCbuf = std::make_unique<PixelConstantBuffer<DirectionalLightCBuf>>(gfx);
	}
	Reset();
}

void DirectionalLight::Reset() noexcept
{
	cbData = {
		{ 0.25f, 0.5f, -1.0f },
	};
}

void DirectionalLight::Bind(Graphics& gfx) noexcept
{
	pCbuf->Update(gfx, cbData);
	pCbuf->Bind(gfx);
}

std::unique_ptr<PixelConstantBuffer<DirectionalLight::DirectionalLightCBuf>> DirectionalLight::pCbuf;