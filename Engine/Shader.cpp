#include "Shader.h"

Shader::Shader()
{
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myInputLayout = nullptr;
	myMatrixBuffer = nullptr;
	mySampleState = nullptr;
}

Shader::Shader(const Shader& aShader)
{
}

Shader::~Shader()
{
}

bool Shader::Initialize(ID3D11Device& aDevice, HWND& aHWND)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(aDevice, aHWND, L"../../Bin/Shaders/vertex_texture.vs", L"../../Bin/Shaders/pixel_texture.ps");
	if (!result)
	{
		return false;
	}
	return true;
}

void Shader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();
}

bool Shader::Render(ID3D11DeviceContext& aDeviceContext, int aIndexCount, XMMATRIX& aWorldMatrix, XMMATRIX& aViewMatrix,
	XMMATRIX& aProjectionMatrix, ID3D11ShaderResourceView& aTexture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix, &aTexture);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(aDeviceContext, aIndexCount);

	return true;
}

bool Shader::InitializeShader(ID3D11Device& aDevice, HWND& aHWND, WCHAR* aVertexShader, WCHAR* aPixelShader)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(aVertexShader, nullptr, nullptr, "VertexShader_Textured", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage != nullptr)
		{
			OutputShaderErrorMessage(errorMessage, aHWND, aVertexShader);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(aHWND, aVertexShader, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(aPixelShader, nullptr, nullptr, "PixelShader_Textured", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage != nullptr)
		{
			OutputShaderErrorMessage(errorMessage, aHWND, aPixelShader);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(aHWND, aPixelShader, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = aDevice.CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &myVertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = aDevice.CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &myPixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = aDevice.CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &myInputLayout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = aDevice.CreateBuffer(&matrixBufferDesc, nullptr, &myMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = aDevice.CreateSamplerState(&samplerDesc, &mySampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Shader::ShutdownShader()
{
	// Release the sampler state.
	if (mySampleState != nullptr)
	{
		mySampleState->Release();
		mySampleState = nullptr;
	}
	// Release the matrix constant buffer.
	if (myMatrixBuffer != nullptr)
	{
		myMatrixBuffer->Release();
		myMatrixBuffer = nullptr;
	}
	// Release the layout.
	if (myInputLayout != nullptr)
	{
		myInputLayout->Release();
		myInputLayout = nullptr;
	}
	// Release the pixel shader.
	if (myPixelShader != nullptr)
	{
		myPixelShader->Release();
		myPixelShader = nullptr;
	}
	// Release the vertex shader.
	if (myVertexShader != nullptr)
	{
		myVertexShader->Release();
		myVertexShader = nullptr;
	}
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* aErrorMessage, HWND& aHWND, WCHAR* aShaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(aErrorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = aErrorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	aErrorMessage->Release();
	aErrorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(aHWND, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFilename, MB_OK);
}

bool Shader::SetShaderParameters(ID3D11DeviceContext& aDeviceContext, XMMATRIX& aWorldMatrix, XMMATRIX& aViewMatrix,
	XMMATRIX& aProjectionMatrix, ID3D11ShaderResourceView* aTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	aWorldMatrix = XMMatrixTranspose(aWorldMatrix);
	aViewMatrix = XMMatrixTranspose(aViewMatrix);
	aProjectionMatrix = XMMatrixTranspose(aProjectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = aDeviceContext.Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = aWorldMatrix;
	dataPtr->view = aViewMatrix;
	dataPtr->projection = aProjectionMatrix;

	// Unlock the constant buffer.
	aDeviceContext.Unmap(myMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	aDeviceContext.VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);

	// Set shader texture resource in the pixel shader.
	aDeviceContext.PSSetShaderResources(0, 1, &aTexture);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext& aDeviceContext, int aIndexCount)
{
	// Set the vertex input layout.
	aDeviceContext.IASetInputLayout(myInputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	aDeviceContext.VSSetShader(myVertexShader, nullptr, 0);
	aDeviceContext.PSSetShader(myPixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	aDeviceContext.PSSetSamplers(0, 1, &mySampleState);

	// Render the triangle.
	aDeviceContext.DrawIndexed(aIndexCount, 0, 0);
}