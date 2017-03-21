#include "Model.h"

Model::Model()
{
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myTexture = nullptr;
}

Model::Model(const Model& aModel)
{
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath)
{
	bool result;

	// Initialize the vertex and index buffers
	result = InitializeBuffers(aDevice);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model
	result = LoadTexture(aDevice, aDeviceContext, aTexturePath);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	// Release the model texture
	ReleaseTexture();

	// Shutdown the vertex and index buffers
	ShutdownBuffers();
}

void Model::Render(ID3D11DeviceContext& aDeviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(aDeviceContext);
}

int Model::GetIndexCount()
{
	return myIndexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return myTexture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device& aDevice)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array
	myVertexCount = 4;

	// Set the number of indices in the index array
	myIndexCount = 6;

	// Create the vertex array
	vertices = new VertexType[myVertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array
	indices = new unsigned long[myIndexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array with data
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top Right
	vertices[3].texture = XMFLOAT2(1.0f, 0.0f);

	// Load the index array with data
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;


	// Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * myVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer
	result = aDevice.CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * myIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = aDevice.CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer
	if (myIndexBuffer)
	{
		myIndexBuffer->Release();
		myIndexBuffer = nullptr;
	}
	// Release the vertex buffer
	if (myVertexBuffer)
	{
		myVertexBuffer->Release();
		myVertexBuffer = nullptr;
	}
}

void Model::RenderBuffers(ID3D11DeviceContext& aDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered
	aDeviceContext.IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered
	aDeviceContext.IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	aDeviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::LoadTexture(ID3D11Device& aDevice, ID3D11DeviceContext& aDeviceContext, const std::string& aTexturePath)
{
	bool result;

	// Create the texture object
	myTexture = new Texture;
	if (!myTexture)
	{
		return false;
	}

	// Initialize the texture object
	result = myTexture->Initialize(aDevice, aDeviceContext, aTexturePath);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	// Release the texture object
	if (myTexture != nullptr)
	{
		myTexture->Shutdown();
		delete myTexture;
		myTexture = nullptr;
	}
}