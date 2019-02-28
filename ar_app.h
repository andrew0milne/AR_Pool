#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <platform/vita/graphics/texture_vita.h>

#include "primitive_builder.h"
#include "../build/vs2017/GameObject.h"
#include "../build/vs2017/Camera.h"
#include "../build/vs2017/Table.h"
#include "../build/vs2017/Physics.h"
#include "../build/vs2017/Menu.h"

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class InputManager;	
}



class ARApp : public gef::Application
{
public:
	ARApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();

private:
	void InitBalls();
	
	void Collisions();
	bool CueCollision();

	bool BallsMoving();
	void EndGame(Tag tag);
	bool CountBalls();

	void InitFont();
	void CleanUpFont();
	void DrawHUD();

	void RenderOverlay();
	void SetupLights();

	void AddScore(Tag tag);
	
	void ProcessControllerInput();
	


	gef::InputManager* input_manager_;
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	gef::SpriteRenderer* sprite_renderer_;
	
	gef::Font* font_;

	float fps_;

	class gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;

	gef::Sprite camera_image_sprite_;
	gef::TextureVita* camera_image_texture_;

	Physics* physics;

	Table* table_;

	static const int number_of_balls = 6;

	GameObject* balls_[number_of_balls];

	GameObject* player_ball;

	float y_scaling_factor_;

	bool marker_visable_;

	bool table_lock_;

	State state;
	GameState game_state;

	Menu* menu;

	bool marker_found_;

	int yellow_score;
	int red_score;
	int max_score_;

	#define MARKER_SIZE_ 0.059F
};




#endif // _RENDER_TARGET_APP_H