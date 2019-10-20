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
		//Structure for vertex
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			}pos;

			struct
			{
				float u;
				float v;
			};
		};

		//Create the vertex buffer
		const std::vector<Vertex> vertices =
		{
			{-1.0f,-1.0f,-1.0f},
			{ 1.0f,-1.0f,-1.0f},
			{-1.0f, 1.0f,-1.0f},
			{ 1.0f, 1.0f,-1.0f},
			{-1.0f,-1.0f, 1.0f},
			{ 1.0f,-1.0f, 1.0f},
			{-1.0f, 1.0f, 1.0f},
			{ 1.0f, 1.0f, 1.0f},
		};

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		//Create vertex shader
		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
		auto pVertexShaderBytecode = pVertexShader->GetBytecode();
		AddStaticBind(std::move(pVertexShader));

		//Create pixel shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));

		//Create index buffer
		const std::vector<unsigned short> indices =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};
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
			}
		};
		//Bind Constant buffer to the pixel shader
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		//Define Input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
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
	eularX += angle;
}

void Box::SetEularY(float angle)
{
	eularY += angle;
}

void Box::SetEularZ(float angle)
{
	eularZ += angle;
}

void Box::Update(float dt) noexcept
{
	xPos += xVel * dt;
	yPos += yVel * dt;
	zPos += zVel * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return 	DirectX::XMMatrixTranslation(xPos, yPos, zPos) *
		DirectX::XMMatrixRotationX(eularX) *
		DirectX::XMMatrixRotationY(eularY) *
		DirectX::XMMatrixRotationZ(eularZ);
}
