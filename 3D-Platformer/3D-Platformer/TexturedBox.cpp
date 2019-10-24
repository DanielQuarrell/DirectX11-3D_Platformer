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

TexturedBox::TexturedBox(Graphics& gfx, std::wstring _textureName, float _x, float _y, float _z) :
	textureName(_textureName),
	xPos(_x),
	yPos(_y),
	zPos(_z)
{
	if (!IsStaticInitialised())
	{


		//Create the vertex buffer
		const float side = 0.5f;
		vertices = std::vector<Vertex>
		{
			// Front Face
			Vertex(-side, -side, -side, 0.0f,  1.0f),
			Vertex(-side,  side, -side, 0.0f,  0.0f),
			Vertex( side,  side, -side, 1.0f,  0.0f),
			Vertex( side, -side, -side, 1.0f,  1.0f),
										   
			// Back Face			   
			Vertex(-side, -side,  side, 1.0f,  1.0f),
			Vertex( side, -side,  side, 0.0f,  1.0f),
			Vertex( side,  side,  side, 0.0f,  0.0f),
			Vertex(-side,  side,  side, 1.0f,  0.0f),
											
			// Top Face							
			Vertex(-side,  side, -side,  0.0f, 1.0f),
			Vertex(-side,  side,  side,  0.0f, 0.0f),
			Vertex( side,  side,  side,  1.0f, 0.0f),
			Vertex( side,  side, -side,  1.0f, 1.0f),
									
			// Bottom Face			
			Vertex(-side, -side, -side,  1.0f, 1.0f),
			Vertex( side, -side, -side,  0.0f, 1.0f),
			Vertex( side, -side,  side,  0.0f, 0.0f),
			Vertex(-side, -side,  side,  1.0f, 0.0f),
									
			// Left Face			
			Vertex(-side, -side,  side,  0.0f, 1.0f),
			Vertex(-side,  side,  side,  0.0f, 0.0f),
			Vertex(-side,  side, -side,  1.0f, 0.0f),
			Vertex(-side, -side, -side,  1.0f, 1.0f),
									
			// Right Face			
			Vertex( side, -side, -side,  0.0f, 1.0f),
			Vertex( side,  side, -side,  0.0f, 0.0f),
			Vertex( side,  side,  side,  1.0f, 0.0f),
			Vertex( side, -side,  side,  1.0f, 1.0f),
		};

		//Create index buffer
		std::vector<unsigned short> indices =
		{
			// Front Face
			0,  1,  2,
			0,  2,  3,

			// Back Face
			4,  5,  6,
			4,  6,  7,

			// Top Face
			8,  9, 10,
			8, 10, 11,

			// Bottom Face
			12, 13, 14,
			12, 14, 15,

			// Left Face
			16, 17, 18,
			16, 18, 19,

			// Right Face
			20, 21, 22,
			20, 22, 23
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

			const DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p0), DirectX::XMVectorSubtract(p2, p0)));

			XMStoreFloat3(&v0.normals, n);
			XMStoreFloat3(&v1.normals, n);
			XMStoreFloat3(&v2.normals, n);
		}

		//Bind vertex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		//Bind texture from file
		std::wstring imagePath = L"Images\\";
		AddStaticBind(std::make_unique<Texture>(gfx, imagePath + textureName));

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
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
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

void TexturedBox::SetPosition(float _x, float _y, float _z)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
}

void TexturedBox::SetEularX(float angle)
{
	xRot = angle;
}

void TexturedBox::SetEularY(float angle)
{
	yRot = angle;
}

void TexturedBox::SetEularZ(float angle)
{
	zRot = angle;
}

void TexturedBox::Update(float dt) noexcept
{

}

DirectX::XMMATRIX TexturedBox::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&modelTransform) *
		DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot) *
		DirectX::XMMatrixTranslation(xPos, yPos, zPos);
}
