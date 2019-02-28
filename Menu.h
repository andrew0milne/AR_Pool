#pragma once
#include <system/platform.h>
#include <graphics/texture.h>
#include <graphics/sprite.h>

#include <input/sony_controller_input_manager.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

enum State
{
	SPLASH, MENU, GAME
};

enum MenuState
{
	SPLASHS, PRESS_X, MAIN, TUTORIAL, PAUSE, QUIT, LOADING, WIN, GAMES
};

enum GameState
{
	MAKE_TABLE, SET_UP, PLAY
};

enum Tag 
{ 
	CUE, RED, YELLOW, WHITE, POCKET, END 
};

class Menu
{
public:
	Menu(gef::Platform& platform_, gef::SpriteRenderer* sprite_renderer_, float scale);
	~Menu();

	void InitMenu(MenuState state);

	bool UpdateSplash(float frame_time);
	bool UpdateMenu(float frame_time, GameState state);
	
	void RenderSplash(gef::SpriteRenderer* sprite_renderer_);
	void RenderMenu(gef::SpriteRenderer* sprite_renderer_, GameState game_state);
	void RenderWarning(gef::SpriteRenderer* sprite_renderer_, GameState game_state);
	void RenderLoading(gef::Platform& platform_, gef::SpriteRenderer* sprite_renderer_);
	void RenderMiniMenu(gef::SpriteRenderer* sprite_renderer_, bool choice, gef::Sprite sprite);

	void RenderHelp(gef::SpriteRenderer* sprite_renderer_, GameState state);

	void RenderEnd(gef::SpriteRenderer* sprite_renderer_);

	void EndSplash();

	void UpdateWinner(int winner);
	void UpdateMarkerFound(bool found);

	void Playing();

	bool IsPaused();

	void Quit();

	void ButtonPressed(UInt32 button_num, State state);

	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);

	bool render_help;

private:
	
	
	gef::Sprite loading_sprite_;	
	gef::Sprite menu_sprite_;
	gef::Sprite menu_back_sprite_;	
	gef::Sprite select_sprite_;	
	gef::Sprite splash_sprite_;

	gef::Sprite tutorial_1_sprite_;
	gef::Sprite tutorial_2_sprite_;
	gef::Sprite tutorial_3_sprite_;	

	gef::Sprite pause_sprite_;
	gef::Sprite r_win_sprite_;	
	gef::Sprite y_win_sprite_;	
	gef::Sprite quit_sprite_;

	gef::Sprite marker1_sprite_;
	gef::Sprite marker2_sprite_;

	gef::Sprite lock_table_;
	gef::Sprite get_cue_back_;
	gef::Sprite this_cue_;

	gef::SpriteRenderer* sprite_renderer_;

	float splash_counter_;
	bool end_splash_;

	bool press_x_;
	bool end_menu_;
	bool winner_found_;
	bool markers_found_;

	bool in_tutorial_;

	bool paused_;

	int menu_choice;
	int tutorial_choice;
	bool pause_choice;
	bool quit_choice;

	float button_delay_timer;
	const float button_delay = 0.1f;
	bool can_press_button;

	int winner;
	float win_timer;

	
	float timer;
	bool start;

	float scaling_factor;

	MenuState menu_state;
};

