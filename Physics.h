#include <system/platform.h>
#include "system\debug_log.h"
#include "../build/vs2017/GameObject.h"

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
class Physics
{
public:
	Physics();
	~Physics();

	float GetDistance(gef::Vector4 pos_1_, gef::Vector4 pos_2_);
	bool SphereToRayCollision(GameObject* sphere_, GameObject* wall_, int i);
	bool SphereToSphereCollision(GameObject* object_1_, GameObject* object_2_, bool pocket);
	float DotProduct(gef::Vector4 vec_1_, gef::Vector4 vec_2_);

};


