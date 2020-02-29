#include "Box.h"
//Bindables
#include "ConstantBuffers.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Topology.h"
#include "TransformCbuf.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

Box::Box(Graphics& gfx, float _x, float _y, float _z) :
	xPos(_x),
	yPos(_y),
	zPos(_z)
{
	if (!IsStaticInitialised())
	{
		struct Vertex
		{
			Vertex() {};
			Vertex(
				float x, float y, float z) :
				pos(x, y, z) {}

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 normals;
		};

		//Create the vertex buffer
		const float side = 0.5f;
		std::vector<Vertex> vertices
		{
			// Front Face
			Vertex(-side, -side, -side),
			Vertex(-side,  side, -side),
			Vertex(side,  side, -side),
			Vertex(side, -side, -side),

			// Back Face			   
			Vertex(-side, -side,  side),
			Vertex(side, -side,  side),
			Vertex(side,  side,  side),
			Vertex(-side,  side,  side),

			// Top Face					
			Vertex(-side,  side, -side),
			Vertex(-side,  side,  side),
			Vertex(side,  side,  side),
			Vertex(side,  side, -side),

			// Bottom Face			
			Vertex(-side, -side, -side),
			Vertex(side, -side, -side),
			Vertex(side, -side,  side),
			Vertex(-side, -side,  side),

			// Left Face			
			Vertex(-side, -side,  side),
			Vertex(-side,  side,  side),
			Vertex(-side,  side, -side),
			Vertex(-side, -side, -side),

			// Right Face			
			Vertex(side, -side, -side),
			Vertex(side,  side, -side),
			Vertex(side,  side,  side),
			Vertex(side, -side,  side),
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

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		//Create vertex shader
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
		auto pVertexShaderBytecode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));

		//Create pixel shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));


		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		//Lookup table for cube face colors
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{ 1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,1.0f,1.0f },
			},
		};
		//Bind Constant buffer to the pixel shader
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		//Define Input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
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

void Box::SetVelocity(float _x, float _y, float _z)
{
	xVel = _x;
	yVel = _y;
	zVel = _z;
}

void Box::SetPosition(float _x, float _y, float _z)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
}

void Box::SetEularX(float angle)
{
	xRot += angle;
}

void Box::SetEularY(float angle)
{
	yRot += angle;
}

void Box::SetEularZ(float angle)
{
	zRot += angle;
}

void Box::Update(float dt) noexcept
{
	yRot += dt;

	xPos += xVel * dt;
	yPos += yVel * dt;
	zPos += zVel * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&modelTransform) *
		DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot) *
		DirectX::XMMatrixTranslation(xPos, yPos, zPos);
}
