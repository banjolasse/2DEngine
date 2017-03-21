#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Texture.h"
#include <string>

using namespace DirectX;

class Model
{
public:
	Model();
	Model(const Model& aModel);
	~Model();

	bool Initialize(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath);
	void Shutdown();
	void Render(ID3D11DeviceContext& aDeviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	bool InitializeBuffers(ID3D11Device& aDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext& aDeviceContext);
	bool LoadTexture(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath);
	void ReleaseTexture();

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	Texture* myTexture;
	int myVertexCount;
	int myIndexCount;
};
