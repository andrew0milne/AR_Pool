#include "Camera.h"



Camera::Camera()
{
	// initialise the camera settings
	position_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	lookat_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	fov_ = gef::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 1000.f;
}

Camera::~Camera()
{
}

void Camera::SetPosition(gef::Vector4 pos_)
{
	position_ = pos_;
}

gef::Vector4 Camera::GetPosition()
{
	return position_;
}

gef::Vector4 Camera::GetLookAt()
{
	return lookat_;
}

gef::Vector4 Camera::GetUp()
{
	return up_;
}

float Camera::GetFOV()
{
	return fov_;
}

float Camera::GetNearPlane()
{
	return near_plane_;
}

float Camera::GetFarPlane()
{
	return far_plane_;
}


