#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <string>

class Texture
{
public:
	Texture();
	Texture(const Texture& aTexture);
	~Texture();

	bool Initialize(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool LoadTarga(const std::string& aTexturePath, int& aHeight, int& aWidth);
	
	unsigned char* myTargaData;
	ID3D11Texture2D* myTexture;
	ID3D11ShaderResourceView* myTextureView;
};
