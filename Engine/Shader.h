#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;

class Shader
{
public:
	Shader();
	Shader(const Shader& aShader);
	~Shader();

	bool Initialize(ID3D11Device& aDevice, HWND& aHWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext& aDeviceContext, int aIndexCount, XMMATRIX& aWorldMatrix, XMMATRIX& aViewMatrix,
		XMMATRIX& aProjectionMatrix, ID3D11ShaderResourceView& aTexture);

private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	bool InitializeShader(ID3D11Device& aDevice, HWND& aHWND, WCHAR* aVertexShader, WCHAR* aPixelShader);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* aErrorMessage, HWND& aHWND, WCHAR* aShaderFilename);

	bool SetShaderParameters(ID3D11DeviceContext& aDeviceContext, XMMATRIX& aWorldMatrix, XMMATRIX& aViewMatrix,
		XMMATRIX& aProjectionMatrix, ID3D11ShaderResourceView* aTexture);
	void RenderShader(ID3D11DeviceContext& aDeviceContext, int aIndexCount);

	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
	ID3D11Buffer* myMatrixBuffer;
	ID3D11SamplerState* mySampleState;
};
