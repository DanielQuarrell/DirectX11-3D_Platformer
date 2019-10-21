#pragma once
#include "Bindable.h"

//Constant buffer class to be passed in as a template
template<typename C>

class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& consts)
	{
		//In order to update a constant buffer you need to map a resourse
		//This locks the constant buffer and gets a pointer to the memory

		D3D11_MAPPED_SUBRESOURCE msr = {};
		GetContext(gfx)->Map(
			pConstantBuffer, 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);

		//Then you can write into the memory and call unmap once the process is finished
		memcpy(msr.pData, &consts, sizeof(consts));
		GetContext(gfx)->Unmap(pConstantBuffer, 0u);
	}

	//Constructor with constant buffer initialisation
	ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//Bind to the vertex shader file
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;									//CPU write, GPU read
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						//CPU write as its going to be updated every frame
		constantBufferDesc.ByteWidth = sizeof(consts);
		constantBufferDesc.StructureByteStride = 0u;
		constantBufferDesc.MiscFlags = 0u;


		D3D11_SUBRESOURCE_DATA constantBufferData = {};
		constantBufferData.pSysMem = &consts;
		GetDevice(gfx)->CreateBuffer(&constantBufferDesc, &constantBufferData, &pConstantBuffer);
	}

	//Constructor without constant buffer initialisation
	ConstantBuffer(Graphics& gfx, UINT slot = 0u)
	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//Bind to the vertex shader file
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;									//CPU write, GPU read
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						//CPU write as its going to be updated every frame
		constantBufferDesc.ByteWidth = sizeof(C);
		constantBufferDesc.StructureByteStride = 0u;
		constantBufferDesc.MiscFlags = 0u;

		GetDevice(gfx)->CreateBuffer(&constantBufferDesc, nullptr, &pConstantBuffer);
	}

	//Deconstructor
	~ConstantBuffer()
	{
		if (pConstantBuffer != nullptr)
		{
			pConstantBuffer->Release();
		}
	}

protected:
	ID3D11Buffer* pConstantBuffer = nullptr;
	UINT slot;
};

//Override for vertex buffer
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	//Uses 'using' to gain access to functions with templates
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		//Bind constant buffer to vertex shader
		GetContext(gfx)->VSSetConstantBuffers(slot, 1u, &pConstantBuffer);
	}
};

//Overide for pixel buffer
template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	//Uses 'using' to gain access to parent variables and functions with templates
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::slot;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		//Bind constant buffer to pixel shader
		GetContext(gfx)->PSSetConstantBuffers(slot, 1u, &pConstantBuffer);
	}
};