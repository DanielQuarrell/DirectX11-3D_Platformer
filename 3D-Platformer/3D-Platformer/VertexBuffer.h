#pragma once
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer(Graphics& gfx, const std::vector<V>& vertices) : stride(sizeof(V))
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Use buffer as a vertex buffer
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;									//Reads and writes to the GPU 
		vertexBufferDesc.ByteWidth = UINT(sizeof(V) * vertices.size());					//Byte Size = Vertex struct size * number of elements in the array
		vertexBufferDesc.StructureByteStride = sizeof(V);
		vertexBufferDesc.CPUAccessFlags = 0u;											//Defines how a CPU can access a resource
		vertexBufferDesc.MiscFlags = 0u;

		D3D11_SUBRESOURCE_DATA vertexBufferData = {};										//Clear the memory in the vertex buffer
		vertexBufferData.pSysMem = vertices.data();											//The data to place into the buffer		
		GetDevice(gfx)->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer);	//Create the buffer
	}
	~VertexBuffer();

	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	ID3D11Buffer* pVertexBuffer = nullptr;
};