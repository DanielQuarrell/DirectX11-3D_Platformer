#include "Graphics.h"

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

	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTarget);
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
