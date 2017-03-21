#pragma once

// Libraries to link
#pragma comment(lib, "d3d11.lib") // For setting up and drawing with DirectX 11
#pragma comment(lib, "dxgi.lib") // For getting information about GFX card, refresh rate of monitor, etc.
#pragma comment(lib, "d3dcompiler.lib") // For compiling shaders

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass& aD3DClass);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool myVSyncEnabled;
	int myVideoCardMemory;
	char myVideoCardDescription[128];
	IDXGISwapChain* mySwapChain;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;
	ID3D11RenderTargetView* myRenderTargetView;
	ID3D11Texture2D* myDepthStencilBuffer;
	ID3D11DepthStencilState* myDepthStencilState;
	ID3D11DepthStencilView* myDepthStencilView;
	ID3D11RasterizerState* myRasterState;
	XMMATRIX myProjectionMatrix;
	XMMATRIX myWorldMatrix;
	XMMATRIX myOrthoMatrix;
};
