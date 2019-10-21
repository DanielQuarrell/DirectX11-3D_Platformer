#include "TexturedBox.h"
//Bindables
#include "ConstantBuffers.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuf.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "Texture.h"

TexturedBox::TexturedBox(Graphics& gfx, float _x, float _y, float _z) :
	xPos(_x),
	yPos(_y),
	zPos(_z)
{
	if (!IsStaticInitialised())
	{
		//Structure for vertex
		struct Vertex
		{
			Vertex() {};
			Vertex(
			float x, float y, float z,
			float u, float v) :
			pos(x, y, z), texCoord(u, v){}

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 texCoord;
			DirectX::XMFLOAT3 n;
		};

		//Create the vertex buffer
		const float side = 1.0f;
		std::vector<Vertex> vertices =
		{
			Vertex(-side,-side,-side, 2.0f / 3.0f, 0.0f / 4.0f ),
			Vertex( side,-side,-side, 1.0f / 3.0f, 0.0f / 4.0f ),
			Vertex(-side, side,-side, 2.0f / 3.0f, 1.0f / 4.0f ),
			Vertex( side, side,-side, 1.0f / 3.0f, 1.0f / 4.0f ),
			Vertex(-side,-side, side, 2.0f / 3.0f, 3.0f / 4.0f ),
			Vertex( side,-side, side, 1.0f / 3.0f, 3.0f / 4.0f ),
			Vertex(-side, side, side, 2.0f / 3.0f, 2.0f / 4.0f ),
			Vertex( side, side, side, 1.0f / 3.0f, 2.0f / 4.0f ),
			Vertex(-side,-side,-side, 2.0f / 3.0f, 4.0f / 4.0f ),
			Vertex( side,-side,-side, 1.0f / 3.0f, 4.0f / 4.0f ),
			Vertex(-side,-side,-side, 3.0f / 3.0f, 1.0f / 4.0f ),
			Vertex(-side,-side, side, 3.0f / 3.0f, 2.0f / 4.0f ),
			Vertex( side,-side,-side, 0.0f / 3.0f, 1.0f / 4.0f ),
			Vertex( side,-side, side, 0.0f / 3.0f, 2.0f / 4.0f ),
		};

		//Create index buffer
		std::vector<unsigned short> indices =
		{
			0, 2, 1,   2, 3, 1,
			4, 8, 5,   5, 8, 9,
			2, 6, 3,   3, 6, 7,
			4, 5, 7,   4, 7, 6,
			2, 10,11,  2, 11,6,
			12,3, 7,   12,7, 13
		};

		//Calculate normals for flat object
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];
			const DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.pos);
			const DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.pos);
			const DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2.pos);

			const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::operator-(p1, p0), DirectX::operator-(p2, p0)));

			XMStoreFloat3(&v0.n, n);
			XMStoreFloat3(&v1.n, n);
			XMStoreFloat3(&v2.n, n);
		}

		//Bind vertex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		//Bind texture from file
		AddStaticBind(std::make_unique<Texture>(gfx, L"Images\\cube.png"));

		//Create vertex shader
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
		auto pVertexShaderBytecode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));

		//Create pixel shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));

		//Bind index buffer
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		//Define Input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		//Bind Input vertex layout
		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElementDesc, pVertexShaderBytecode));

		//Set primitive topology to triangle list
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	//Bind transform buffer
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	//Model deformation transform (Instance)
	DirectX::XMStoreFloat3x3(
		&modelTransform,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
	);
}

void TexturedBox::SetVelocity(float _x, float _y, float _z)
{
	xVel = _x;
	yVel = _y;
	zVel = _z;
}

void TexturedBox::SetPosition(float _x, float _y, float _z)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
}

void TexturedBox::SetEularX(float angle)
{
	eularX += angle;
}

void TexturedBox::SetEularY(float angle)
{
	eularY += angle;
}

void TexturedBox::SetEularZ(float angle)
{
	eularZ += angle;
}

void TexturedBox::Update(float dt) noexcept
{
	xPos += xVel * dt;
	yPos += yVel * dt;
	zPos += zVel * dt;
}

DirectX::XMMATRIX TexturedBox::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&modelTransform) *
		DirectX::XMMatrixTranslation(xPos, yPos, zPos) *
		DirectX::XMMatrixRotationX(eularX) *
		DirectX::XMMatrixRotationY(eularY) *
		DirectX::XMMatrixRotationZ(eularZ);
}
