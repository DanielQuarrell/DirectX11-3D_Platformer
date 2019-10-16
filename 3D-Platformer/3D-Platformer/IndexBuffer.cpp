#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices) : count((UINT)indices.size())
{
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;										//Reads and writes to the GPU
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;								//Use buffer as a index buffer
	indexBufferDesc.ByteWidth = UINT(count * sizeof(unsigned short));					//Byte size = DWORD type * Number of triangles * number of verticies
	indexBufferDesc.StructureByteStride = sizeof(unsigned short);
	indexBufferDesc.CPUAccessFlags = 0u;												//Defines how a CPU can access a resource
	indexBufferDesc.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA indexBufferData = {};										//Clear the memory in the index buffer
	indexBufferData.pSysMem = indices.data();											//The data to place into the buffer	
	GetDevice(gfx)->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer);	//Create the buffer	
}

IndexBuffer::~IndexBuffer()
{
	if (pIndexBuffer != nullptr)
	{
		pIndexBuffer->Release();
	}
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	//Bind index buffer to the IA stage of the graphics pipeline
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
