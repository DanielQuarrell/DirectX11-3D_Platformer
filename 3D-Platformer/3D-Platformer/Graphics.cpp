#include "Graphics.h"
#include <sstream>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* pDSState;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	pContext->OMSetDepthStencilState(pDSState, 1u);

	// create depth stensil texture
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

	//Bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, &pRenderTarget, pDSV);

	//Configure viewport
	D3D11_VIEWPORT viewport;

	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	pContext->RSSetViewports(1u, &viewport);

	pDSState->Release();
	pDepthStencil->Release();
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
	if (pDSV != nullptr)
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
	pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawIndexed(UINT count)
{
	pContext->DrawIndexed(count, 0u, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX _projection)
{
	projection = _projection;
}

DirectX::XMMATRIX Graphics::GetProjection() const
{
	return projection;
}
