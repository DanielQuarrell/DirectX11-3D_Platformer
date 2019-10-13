#pragma once
#include <Windows.h>
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();
	void EndFrame();
	void ClearBuffer(float r, float g, float b, float a);
	void DrawTestTriangle(float angle, float x, float y);
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTarget = nullptr;
};