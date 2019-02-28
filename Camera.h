#include <maths\math_utils.h>
#include <math.h>
#include <system/platform.h>


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class Scene;
	class InputManager;
	//class Sphere;
}

#pragma once
class Camera
{
public:
	Camera();
	~Camera();
	
	void Camera::SetPosition(gef::Vector4 pos_);
	
	gef::Vector4 Camera::GetPosition();
	gef::Vector4 Camera::GetLookAt();
	gef::Vector4 Camera::GetUp();
	float Camera::GetFOV();
	float Camera::GetNearPlane();
	float Camera::GetFarPlane();

private:
	gef::Vector4 position_;
	gef::Vector4 lookat_;
	gef::Vector4 up_;
	float fov_;
	float near_plane_;
	float far_plane_;

};

