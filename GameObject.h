#include <graphics/mesh_instance.h>
#include "../primitive_builder.h"
#include <maths\math_utils.h>
#include <math.h>
#include <graphics/scene.h>
#include <graphics\mesh.h>
#include <system/platform.h>
#include <maths\sphere.h>
#include <maths\aabb.h>
#include <graphics/renderer_3d.h>

#include "../build/vs2017/Menu.h"

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
class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	GameObject(gef::Platform& platform_, const char* filename, Tag tag_);
	~GameObject();
	void GameObject::Init(PrimitiveBuilder* pb, UInt32 colour, bool cube);
	gef::Mesh* GameObject::GetFirstMesh(gef::Platform& platform_, gef::Scene* scene);
	
	void CueUpdate(float delta_time_, gef::Matrix44 transform_);
	void Update(float delta_time_);
	void Update(float delta_time_, gef::Matrix44 transform_);

	void Render(gef::Renderer3D* renderer_3d_);

	void SetRotation(gef::Vector4 rot);
	void SetPosition(gef::Vector4 pos);

	void UpdateRotation(float delta_time_);
	void UpdatePosition(float delta_time_);
	void UpdateVelocity(float delta_time_);

	void SetScale(double scl_);
	void SetVelocity(gef::Vector4 vel_);
	void SetVelocity(float speed_, float angle_);
	void SetAngularXVelocity(float x_vel_);
	void SetAngularYVelocity(float y_vel_);
	void SetAngularZVelocity(float z_vel_);
	void SetMass(float mass_);
	void SetLocalOriginOffset(gef::Matrix44 origin);

	void SetActive(bool active);

	void SetRay(gef::Matrix44 left, gef::Matrix44 right, float angle);

	void InvertXVelocity();
	void InvertYVelocity();
	
	gef::Sphere GetBoundingSphere();
	
	gef::Vector4 GetPosition();
	float GetMass();
	float GetSpeed();
	float GetVelocityAngle();
	float GetAngle();
	gef::Vector4 GetVelocity();
	float GetScaleFactor();
	gef::Matrix44 GetLocalOriginOffset();
	gef::Vector4 GetLeftPoint();
	gef::Vector4 GetRightPoint();

	bool GetActive();

	gef::Matrix44 local_origin_offset;

	Tag tag_;

private:

	gef::Scene* model_scene_;

	gef::Sphere bounding_sphere_;
	gef::Aabb bounding_box_;

	gef::Vector4 left_point_;
	gef::Vector4 right_point_;
	float distance;

	gef::Matrix44 scale_matrix_;
	gef::Matrix44 x_rot_matrix_;
	gef::Matrix44 y_rot_matrix_;
	gef::Matrix44 z_rot_matrix_;
	gef::Matrix44 transform_matrix_;

	float x_rot_, y_rot_, z_rot_;
	float x_angular_vel_, y_angular_vel_, z_angular_vel_;
	float scale_factor_;
	float mass_;
	gef::Vector4 scale_;
	gef::Vector4 position_;
	gef::Vector4 velocity_;

	gef::Vector4 previous_position;

	gef::Material mat_;
	gef::Material move_mat_;

	float angle;

	bool is_ball;

	float friction_;

	bool active_;
	
	
};


