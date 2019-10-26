#include "GameObject.h"
#include "IndexBuffer.h"

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
	gfx.DrawIndexed(pIndexBuffer->GetCount());
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
	centerOffset = DirectX::XMVectorSet(maxVertex.x - halfDistX, maxVertex.y - halfDistY, maxVertex.z - halfDistZ, 0.0f);

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

	unsigned short* i = new unsigned short[36];

	// Front Face
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Back Face
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Top Face
	i[12] = 1; i[13] = 7; i[14] = 6;
	i[15] = 1; i[16] = 6; i[17] = 2;

	// Bottom Face
	i[18] = 0; i[19] = 4; i[20] = 5;
	i[21] = 0; i[22] = 5; i[23] = 3;

	// Left Face
	i[24] = 4; i[25] = 7; i[26] = 1;
	i[27] = 4; i[28] = 1; i[29] = 0;

	// Right Face
	i[30] = 3; i[31] = 2; i[32] = 6;
	i[33] = 3; i[34] = 6; i[35] = 5;

	for (int j = 0; j < 36; j++)
	{
		bbIndices.push_back(i[j]);
	}
}

void GameObject::CalculateAABB(DirectX::XMMATRIX transformMatrix)
{
	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	//Loop through the 8 vertices describing the bounding box
	for (UINT i = 0; i < bbVertices.size(); i++)
	{
		//Transform the bounding boxes vertices to the objects world space
		DirectX::XMVECTOR Vert = DirectX::XMVectorSet(bbVertices[i].x, bbVertices[i].y, bbVertices[i].z, 0.0f);
		Vert = DirectX::XMVector3TransformCoord(Vert, transformMatrix);

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, DirectX::XMVectorGetX(Vert));	// Find smallest x value in model
		minVertex.y = min(minVertex.y, DirectX::XMVectorGetY(Vert));	// Find smallest y value in model
		minVertex.z = min(minVertex.z, DirectX::XMVectorGetZ(Vert));	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, DirectX::XMVectorGetX(Vert));	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, DirectX::XMVectorGetY(Vert));	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, DirectX::XMVectorGetZ(Vert));	// Find largest z value in model
	}

	//Store Bounding Box's min and max vertices
	bbMinVertex = DirectX::XMVectorSet(minVertex.x, minVertex.y, minVertex.z, 0.0f);
	bbMaxVertex = DirectX::XMVectorSet(maxVertex.x, maxVertex.y, maxVertex.z, 0.0f);
}

DirectX::XMVECTOR GameObject::GetBBMinVertex()
{
	return bbMinVertex;
}

DirectX::XMVECTOR GameObject::GetBBMaxVertex()
{
	return bbMaxVertex;
}