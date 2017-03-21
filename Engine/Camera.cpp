#include "Camera.h"

Camera::Camera()
{
	myPositionX = 0.0f;
	myPositionY = 0.0f;
	myPositionZ = 0.0f;

	myRotationX = 0.0f;
	myRotationY = 0.0f;
	myRotationZ = 0.0f;
}

Camera::Camera(const Camera& aCamera)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(const float aX, const float aY, const float aZ)
{
	myPositionX = aX;
	myPositionY = aY;
	myPositionZ = aZ;
}

void Camera::SetRotation(const float aX, const float aY, const float aZ)
{
	myRotationX = aX;
	myRotationY = aY;
	myRotationZ = aZ;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(myPositionX, myPositionY, myPositionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(myRotationX, myRotationY, myRotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = myPositionX;
	position.y = myPositionY;
	position.z = myPositionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = myRotationX * 0.0174532925f;
	yaw = myRotationY * 0.0174532925f;
	roll = myRotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	myViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::GetViewMatrix(XMMATRIX& aViewMatrix)
{
	aViewMatrix = myViewMatrix;
}