#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	~IndexBuffer();
	void Bind(Graphics& gfx) noexcept override;
	UINT GetCount() const noexcept;
protected:
	UINT count;
	ID3D11Buffer* pIndexBuffer = nullptr;
};