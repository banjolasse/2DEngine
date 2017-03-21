#pragma once

#include <directxmath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera& aCamera);
	~Camera();

	void SetPosition(const float aX, const float aY, const float aZ);
	void SetRotation(const float aX, const float aY, const float aZ);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& aViewMatrix);

private:
	float myPositionX;
	float myPositionY;
	float myPositionZ;
	float myRotationX;
	float myRotationY;
	float myRotationZ;
	XMMATRIX myViewMatrix;
};
