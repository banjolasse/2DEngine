#include "Texture.h"

Texture::Texture()
{
	myTargaData = nullptr;
	myTexture = nullptr;
	myTextureView = nullptr;
}

Texture::Texture(const Texture& aTexture)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath)
{
	bool result;
	int height;
	int width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Load the targa image data into memory.
	result = LoadTarga(aTexturePath.c_str(), height, width);
	if (!result)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = aDevice.CreateTexture2D(&textureDesc, nullptr, &myTexture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	aDeviceContext.UpdateSubresource(myTexture, 0, nullptr, myTargaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = aDevice.CreateShaderResourceView(myTexture, &srvDesc, &myTextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	aDeviceContext.GenerateMips(myTextureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] myTargaData;
	myTargaData = nullptr;

	return true;
}

void Texture::Shutdown()
{
	// Release the texture view resource.
	if (myTextureView != nullptr)
	{
		myTextureView->Release();
		myTextureView = nullptr;
	}
	// Release the texture.
	if (myTexture != nullptr)
	{
		myTexture->Release();
		myTexture = nullptr;
	}
	// Release the targa data.
	if (myTargaData != nullptr)
	{
		delete[] myTargaData;
		myTargaData = nullptr;
	}
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return myTextureView;
}

bool Texture::LoadTarga(const std::string& aTexturePath, int& aHeight, int& aWidth)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, aTexturePath.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	aHeight = (int)targaFileHeader.height;
	aWidth = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = aWidth * aHeight * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	myTargaData = new unsigned char[imageSize];
	if (!myTargaData)
	{
		return false;
	}

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (aWidth * aHeight * 4) - (aWidth * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < aHeight; j++)
	{
		for (i = 0; i < aWidth; i++)
		{
			myTargaData[index + 0] = targaImage[k + 2];  // Red.
			myTargaData[index + 1] = targaImage[k + 1];  // Green.
			myTargaData[index + 2] = targaImage[k + 0];  // Blue
			myTargaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (aWidth * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}