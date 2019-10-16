#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

class Graphics
{
	friend class Bindable;
public:
	Graphics(HWND hWnd);
	~Graphics();
	void EndFrame();
	void ClearBuffer(float r, float g, float b, float a);
	void DrawIndexed(UINT count);
	void SetProjection(DirectX::FXMMATRIX _projection);
	DirectX::XMMATRIX GetProjection() const;
private:
	DirectX::XMMATRIX projection;
	
	//COM objects
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pRenderTarget = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
};