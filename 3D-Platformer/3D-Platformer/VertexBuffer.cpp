#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
	if (pVertexBuffer != nullptr)
	{
		pVertexBuffer->Release();
	}
}

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
	//Bind vertex buffer to pipeline
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);
}
