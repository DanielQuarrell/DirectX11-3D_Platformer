#include "GameObject.h"
#include "IndexBuffer.h"
#include <string>
#include <iostream>

void GameObject::Draw(Graphics& gfx) const noexcept
{
	//Loop through the bindables and bind them to the pipeline
	for (auto& bindable : binds)
	{
		bindable->Bind(gfx);
	}
	for (auto& staticBindable : GetStaticBinds())
	{
		staticBindable->Bind(gfx);
	}

	//Draw object
	if (isVisible)
	{
		gfx.DrawIndexed(pIndexBuffer->GetCount());
	}
}

void GameObject::AddBind(std::unique_ptr<Bindable> bind)
{
	//MUST use AddIndexBuffer to bind index buffer
	binds.push_back(std::move(bind));
}

void GameObject::AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer)
{
	//Don't add index buffer twice
	pIndexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}

void GameObject::CreateBoundingBox(std::vector<DirectX::XMFLOAT3>& vertPosArray)
{
	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (UINT i = 0; i < vertPosArray.size(); i++)
	{
		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].z);    // Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].z);    // Find largest z value in model
	}

	// Compute distance between maxVertex and minVertex
	float halfDistX = (maxVertex.x - minVertex.x) / 2.0f;
	float halfDistY = (maxVertex.y - minVertex.y) / 2.0f;
	float halfDistZ = (maxVertex.z - minVertex.z) / 2.0f;

	// Now store the distance between (0, 0, 0) in model space to the models real center
	centerVertex = DirectX::XMVectorSet(maxVertex.x - halfDistX, maxVertex.y - halfDistY, maxVertex.z - halfDistZ, 0.0f);

	// Create bounding box    
	// Front Vertices
	bbVertices.push_back(DirectX::XMFLOAT3(minVertex.x, minVertex.y, minVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(minVertex.x, maxVertex.y, minVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(maxVertex.x, maxVertex.y, minVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(maxVertex.x, minVertex.y, minVertex.z));

	// Back Vertices
	bbVertices.push_back(DirectX::XMFLOAT3(minVertex.x, minVertex.y, maxVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(maxVertex.x, minVertex.y, maxVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(maxVertex.x, maxVertex.y, maxVertex.z));
	bbVertices.push_back(DirectX::XMFLOAT3(minVertex.x, maxVertex.y, maxVertex.z));
}

void GameObject::CalculateAABB(DirectX::XMMATRIX transformMatrix)
{
	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	//Loop through the 8 vertices describing the bounding box
	for (UINT i = 0; i < bbVertices.size(); i++)
	{
		//Transform the bounding boxes vertices to the objects world space
		DirectX::XMVECTOR vert = DirectX::XMVectorSet(bbVertices[i].x, bbVertices[i].y, bbVertices[i].z, 1.0f);
		vert = DirectX::XMVector3TransformCoord(vert, transformMatrix);

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, DirectX::XMVectorGetX(vert));
		minVertex.y = min(minVertex.y, DirectX::XMVectorGetY(vert));
		minVertex.z = min(minVertex.z, DirectX::XMVectorGetZ(vert));

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, DirectX::XMVectorGetX(vert));
		maxVertex.y = max(maxVertex.y, DirectX::XMVectorGetY(vert));
		maxVertex.z = max(maxVertex.z, DirectX::XMVectorGetZ(vert));
	}

	//Compute distance between maxVertex and minVertex
	bbWidth = (maxVertex.x - minVertex.x);
	bbHeight = (maxVertex.y - minVertex.y);
	bbDepth = (maxVertex.z - minVertex.z);

	float halfDistX = bbWidth  / 2.0f;
	float halfDistY = bbHeight / 2.0f;
	float halfDistZ = bbDepth  / 2.0f;

	// Now store the distance between (0, 0, 0) in model space to the models real center
	centerVertex = DirectX::XMVectorSet(maxVertex.x - halfDistX, maxVertex.y - halfDistY, maxVertex.z - halfDistZ, 0.0f);

	//Store Bounding Box's min and max vertices
	bbMinVertex = DirectX::XMVectorSet(minVertex.x, minVertex.y, minVertex.z, 0.0f);
	bbMaxVertex = DirectX::XMVectorSet(maxVertex.x, maxVertex.y, maxVertex.z, 0.0f);
}

DirectX::XMVECTOR GameObject::GetPosition()
{
	return DirectX::XMVectorSet(xPos, yPos, zPos, 0.0f);
}

DirectX::XMVECTOR GameObject::GetBBMinVertex()
{
	return bbMinVertex;
}

DirectX::XMVECTOR GameObject::GetBBMaxVertex()
{
	return bbMaxVertex;
}

DirectX::XMVECTOR GameObject::GetCenterVertex()
{
	return centerVertex;
}

void GameObject::SetPosition(float _x, float _y, float _z)
{
	xPos = _x;
	yPos = _y;
	zPos = _z;
}

void GameObject::SetVisibility(bool _isVisible)
{
	isVisible = _isVisible;
}
