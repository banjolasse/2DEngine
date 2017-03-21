#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
	myCamera = nullptr;
	myModel = nullptr;
	myShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& aGraphicsClass)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int aScreenWidth, int aScreenHeight, HWND& aHWND)
{
	bool result;

	// Create the Direct3D object.
	myDirect3D = new D3DClass;
	if (!myDirect3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = myDirect3D->Initialize(aScreenWidth, aScreenHeight, VSYNC_ENABLED, aHWND, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(aHWND, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	myCamera = new Camera;
	if (!myCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	myCamera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create the model object.
	myModel = new Model;
	if (!myModel)
	{
		return false;
	}

	// Initialize the model object.
	result = myModel->Initialize(*myDirect3D->GetDevice(), *myDirect3D->GetDeviceContext(), "../../Bin/Sprites/testTexture.tga");
	if (!result)
	{
		MessageBox(aHWND, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the shader object.
	myShader = new Shader;
	if (!myShader)
	{
		return false;
	}

	// Initialize the shader object.
	result = myShader->Initialize(*myDirect3D->GetDevice(), aHWND);
	if (!result)
	{
		MessageBox(aHWND, L"Could not initialize the shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the shader object.
	if (myShader != nullptr)
	{
		myShader->Shutdown();
		delete myShader;
		myShader = nullptr;
	}
	// Release the model object.
	if (myModel != nullptr)
	{
		myModel->Shutdown();
		delete myModel;
		myModel = nullptr;
	}
	// Release the camera object.
	if (myCamera != nullptr)
	{
		delete myCamera;
		myCamera = nullptr;
	}
	// Release the Direct3D object.
	if (myDirect3D != nullptr)
	{
		myDirect3D->Shutdown();
		delete myDirect3D;
		myDirect3D = nullptr;
	}
	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	myCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	myDirect3D->GetWorldMatrix(worldMatrix);
	myCamera->GetViewMatrix(viewMatrix);
	myDirect3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	myModel->Render(*myDirect3D->GetDeviceContext());

	// Render the model using the shader.
	result = myShader->Render(*myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, *myModel->GetTexture());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	myDirect3D->EndScene();
	return true;
}
