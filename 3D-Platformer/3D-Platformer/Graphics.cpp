#include "Graphics.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

Graphics::Graphics(HWND hWnd)
{
	//Swap Chain Buffer Description
	DXGI_MODE_DESC bufferDesc = {};
	bufferDesc.Width = 0;												//When set to 0, 0 direct X figures it out
	bufferDesc.Height = 0;
	bufferDesc.RefreshRate.Numerator = 0;								//Refresh rate in hertz - 60/1 - 60hz
	bufferDesc.RefreshRate.Denominator = 0;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						//Use 32-bit colour
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //Scanline order is unspecified.
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					//Unspecified image scaling 

	//Swap Chain Description - struct

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = bufferDesc;							//Buffer Description
	swapChainDesc.SampleDesc.Count = 1;								//Number of multisamples
	swapChainDesc.SampleDesc.Quality = 0;							//The image quality level. The higher the quality, the lower the performance. The valid range is between zero and one less than the level returned by ID3D11Device::CheckMultisampleQualityLevels
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//How the swap chain is going to be used
	swapChainDesc.BufferCount = 1;									//Number of back buffers in the chain
	swapChainDesc.OutputWindow = hWnd;								//Handle to app window
	swapChainDesc.Windowed = TRUE;									//Windowed or full screen
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			//Lets the display driver handle the front buffer before swapping it out with the back buffer

	//Create the Swapchain
	D3D11CreateDeviceAndSwapChain(
		nullptr,								//Pointer to the Graphics Card / Video adapter to use
		D3D_DRIVER_TYPE_HARDWARE,				//Parameter to determine if Hardware or Software should be used for rendering
		nullptr,								//HMODULE handle to a DLL that's used to impliment software rasterizing
		NULL,									//Flags for different modes such as debugging
		nullptr,								//Pointer for pFeatureLevels (video card features), NULL for highest
		0,										//Number of elements in the pFeatureLevels array
		D3D11_SDK_VERSION,						//SDK version
		&swapChainDesc,
		&pSwapChain,
		&pDevice,
		nullptr,								//Pointer to D3D_FEATURE_LEVEL (Used for backwards compatibility)
		&pContext);

	ID3D11Resource* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTarget);

	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
	if (pSwapChain != nullptr)
	{
		pSwapChain->Release();
	}
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pRenderTarget != nullptr)
	{
		pRenderTarget->Release();
	}
}

void Graphics::EndFrame()
{
	pSwapChain->Present(1, 0);
}

void Graphics::ClearBuffer(float r, float g, float b, float a)
{
	const float color[] = { r,g,b,a };
	pContext->ClearRenderTargetView(pRenderTarget, color);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	//hResult for error checking
	HRESULT hr;

	//Structure for vertex
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	//Create the vertex buffer
	Vertex vertices[] =
	{
		{1.0f,-1.0f,-1.0f},
		{1.0f,-1.0f,-1.0f},
		{1.0f, 1.0f,-1.0f},
		{1.0f, 1.0f,-1.0f},
		{1.0f,-1.0f, 1.0f},
		{1.0f,-1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	ID3D11Buffer* pVertexBuffer;
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;									//Reads and writes to the GPU 
	vertexBufferDesc.ByteWidth = sizeof(vertices);									//Byte Size = Vertex struct * 3 since there is 3 elements in the array
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);							
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Use buffer as a vertex buffer
	vertexBufferDesc.CPUAccessFlags = 0;											//Defines how a CPU can access a resource
	vertexBufferDesc.MiscFlags = 0;						  
														  
	D3D11_SUBRESOURCE_DATA vertexBufferData = {};									//Clear the memory in the vertex buffer
	vertexBufferData.pSysMem = vertices;											//The data to place into the buffer		
	pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer);	//Create the buffer

	//Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);


	//Create index buffer
	DWORD indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	
	ID3D11Buffer* pIndexBuffer;
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;									//Reads and writes to the GPU
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;							//Use buffer as a index buffer
	indexBufferDesc.ByteWidth = sizeof(indices);									//Byte size = DWORD type * Number of triangles * number of verticies
	indexBufferDesc.StructureByteStride = sizeof(DWORD);
	indexBufferDesc.CPUAccessFlags = 0;												//Defines how a CPU can access a resource
	indexBufferDesc.MiscFlags = 0;													

	D3D11_SUBRESOURCE_DATA indexBufferData = {};									//Clear the memory in the index buffer
	indexBufferData.pSysMem = indices;												//The data to place into the buffer	
	pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer);		//Create the buffer	

	//Bind index buffer to the IA stage of the graphics pipeline
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(angle) *
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixTranslation(x,y,4.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f ,0.5f,10.0f)
			)
		}
	};
	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;									//CPU write, GPU read
	constantBufferDesc.ByteWidth = sizeof(cb);
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//Bind to the vertex shader file
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						//CPU write as its going to be updated every frame
	constantBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA constantBufferData = {};
	constantBufferData.pSysMem = &cb;
	pDevice->CreateBuffer(&constantBufferDesc, &constantBufferData, &pConstantBuffer);

	//Bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

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
			{1.0f,0.0f,1.0f,1.0f},
			{1.0f,0.0f,0.0f,1.0f},
			{0.0f,1.0f,0.0f,1.0f},
			{0.0f,0.0f,1.0f,1.0f},
			{1.0f,1.0f,0.0f,1.0f},
			{0.0f,1.0f,1.0f,1.0f},
		}
	};

	ID3D11Buffer* pConstantBuffer2;
	D3D11_BUFFER_DESC constantBuffer2Desc;
	constantBuffer2Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//Bind to the pixel shader file
	constantBuffer2Desc.Usage = D3D11_USAGE_DEFAULT;
	constantBuffer2Desc.ByteWidth = sizeof(cb2);
	constantBuffer2Desc.StructureByteStride = 0;
	constantBuffer2Desc.CPUAccessFlags = 0;
	constantBuffer2Desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA constantBuffer2Data = {};
	constantBuffer2Data.pSysMem = &cb2;

	pDevice->CreateBuffer(&constantBuffer2Desc, &constantBuffer2Data, &pConstantBuffer2);

	//Bind constant buffer to pixel shader
	pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer2);

	//Create pixel shader
	ID3D11PixelShader* pPixelShader;
	ID3DBlob* pPixelBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &pPixelBlob);
	pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, &pPixelShader);

	//Bind pixel shader
	pContext->PSSetShader(pPixelShader, nullptr, 0u);


	//Create vertex shader
	ID3D11VertexShader* pVertexShader;
	ID3DBlob* pVertexBlob;
	D3DReadFileToBlob(L"VertexShader.cso", &pVertexBlob);
	pDevice->CreateVertexShader(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), nullptr, &pVertexShader);

	//Bind vertex shader
	pContext->VSSetShader(pVertexShader, nullptr, 0u);


	//Define Input layout
	ID3D11InputLayout* pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	
	pDevice->CreateInputLayout(
		inputElementDesc, std::size(inputElementDesc),
		pVertexBlob->GetBufferPointer(),
		pVertexBlob->GetBufferSize(),
		&pInputLayout
	);

	//Bind vertex layout
	pContext->IASetInputLayout(pInputLayout);

	//Bind render target
	pContext->OMSetRenderTargets(1, &pRenderTarget, nullptr);

	//Set primitive topology to triangle list
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Configure viewport
	D3D11_VIEWPORT viewport;

	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	pContext->RSSetViewports(1u, &viewport);

	pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);

	//Release Objects
	pVertexBuffer->Release();
	pIndexBuffer->Release();
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexBlob->Release();
	pPixelBlob->Release();
	pInputLayout->Release();
	pConstantBuffer->Release();
	pConstantBuffer2->Release();
}
