#pragma once

#include "d3dclass.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& aGraphicsClass);
	~GraphicsClass();

	bool Initialize(int aScreenWidth, int aScreenHeight, HWND& aHWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

	D3DClass* myDirect3D;
	Camera* myCamera;
	Model* myModel;
	Shader* myShader;
};
