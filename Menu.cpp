#include "Menu.h"

#include <graphics/sprite_renderer.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <cstdlib>

#include <system/application.h>
#include <system/platform.h>

#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <maths/math_utils.h>

#include <iostream>

#include <stdlib.h>
#include <time.h> 

Menu::Menu(gef::Platform& platform_, gef::SpriteRenderer* sprite_renderer_, float scale)
{
	// loads and renders the 'loading, please wait' screen
	loading_sprite_.set_texture(CreateTextureFromPNG("LoadingScreen.png", platform_));
	loading_sprite_.set_position(480.0f, 272.0f, 0.0f);
	loading_sprite_.set_height(544.0f);
	loading_sprite_.set_width(960.0f);

	RenderLoading(platform_, sprite_renderer_);


	//Loads in the rest of the images
	marker1_sprite_.set_texture(CreateTextureFromPNG("Warning.png", platform_));
	marker1_sprite_.set_position(480.0f, 272.0f, 0.0f);
	marker1_sprite_.set_height(544.0f);
	marker1_sprite_.set_width(960.0f);

	marker2_sprite_.set_texture(CreateTextureFromPNG("Warning.png", platform_));
	marker2_sprite_.set_position(480.0f, 272.0f, 0.0f);
	marker2_sprite_.set_height(544.0f);
	marker2_sprite_.set_width(960.0f);

	splash_sprite_.set_texture(CreateTextureFromPNG("SplashScreen.png", platform_));
	splash_sprite_.set_position(480.0f, 272.0f, 0.0f);
	splash_sprite_.set_height(544.0f);
	splash_sprite_.set_width(960.0f);

	menu_sprite_.set_texture(CreateTextureFromPNG("MainMenu.png", platform_));
	menu_sprite_.set_position(480.0f, 272.0f, 0.0f);
	menu_sprite_.set_height(544.0f);
	menu_sprite_.set_width(960.0f);

	menu_back_sprite_.set_texture(CreateTextureFromPNG("MenuBack.png", platform_));
	menu_back_sprite_.set_position(480.0f, 272.0f, 0.0f);
	menu_back_sprite_.set_height(544.0f);
	menu_back_sprite_.set_width(960.0f);

	tutorial_1_sprite_.set_texture(CreateTextureFromPNG("Tutorial1.png", platform_));
	tutorial_1_sprite_.set_position(480.0f, 272.0f, 0.0f);
	tutorial_1_sprite_.set_height(544.0f);
	tutorial_1_sprite_.set_width(960.0f);

	tutorial_2_sprite_.set_texture(CreateTextureFromPNG("Tutorial2.png", platform_));
	tutorial_2_sprite_.set_position(480.0f, 272.0f, 0.0f);
	tutorial_2_sprite_.set_height(544.0f);
	tutorial_2_sprite_.set_width(960.0f);

	tutorial_3_sprite_.set_texture(CreateTextureFromPNG("Tutorial3.png", platform_));
	tutorial_3_sprite_.set_position(480.0f, 272.0f, 0.0f);
	tutorial_3_sprite_.set_height(544.0f);
	tutorial_3_sprite_.set_width(960.0f);

	select_sprite_.set_texture(CreateTextureFromPNG("Select.png", platform_));
	select_sprite_.set_position(350.0f, 250.0f, 0.0f);
	select_sprite_.set_height(22.0f);
	select_sprite_.set_width(22.0f);

	pause_sprite_.set_texture(CreateTextureFromPNG("PauseMenu.png", platform_));
	pause_sprite_.set_position(0.0f, 0.0f, 0.0f);
	pause_sprite_.set_height(2.0f);
	pause_sprite_.set_width(2.0f);

	r_win_sprite_.set_texture(CreateTextureFromPNG("RedWin.png", platform_));
	r_win_sprite_.set_position(480.0f, 272.0f, 0.0f);
	r_win_sprite_.set_height(544.0f);
	r_win_sprite_.set_width(960.0f);

	y_win_sprite_.set_texture(CreateTextureFromPNG("YellowWin.png", platform_));
	y_win_sprite_.set_position(480.0f, 272.0f, 0.0f);
	y_win_sprite_.set_height(544.0f);
	y_win_sprite_.set_width(960.0f);

	quit_sprite_.set_texture(CreateTextureFromPNG("QuitMenu.png", platform_));
	quit_sprite_.set_position(480.0f, 272.0f, 0.0f);
	quit_sprite_.set_height(544.0f);
	quit_sprite_.set_width(960.0f);

	lock_table_.set_texture(CreateTextureFromPNG("PressX.png", platform_));
	lock_table_.set_position(480.0f, 272.0f, 0.0f);
	lock_table_.set_height(544.0f);
	lock_table_.set_width(960.0f);

	get_cue_back_.set_texture(CreateTextureFromPNG("PressTri.png", platform_));
	get_cue_back_.set_position(480.0f, 272.0f, 0.0f);
	get_cue_back_.set_height(544.0f);
	get_cue_back_.set_width(960.0f);

	this_cue_.set_texture(CreateTextureFromPNG("ThisCue.png", platform_));
	this_cue_.set_position(480.0f, 272.0f, 0.0f);
	this_cue_.set_height(544.0f);
	this_cue_.set_width(960.0f);


	splash_counter_ = 0.0f;
	end_splash_ = false;
	press_x_ = false;
	
	scaling_factor = scale;

	InitMenu(PRESS_X);
}

Menu::~Menu()
{
}

// Initialises the various menu variables, so the menu can be reset
void Menu::InitMenu(MenuState state)
{
	end_menu_ = false;
	winner_found_ = false;
	in_tutorial_ = false;
	paused_ = false;
	pause_choice = true;
	quit_choice = true;
	can_press_button = true;
	start = true;
	render_help = true;

	menu_choice = 0;
	tutorial_choice = 0;
	winner = -1;

	menu_state = state;

	button_delay_timer = 0.0f;
	win_timer = 0.0f;
	timer = 0.0f;

	select_sprite_.set_height(22.0f);
	select_sprite_.set_width(22.0f);
}

// Updates the counter timer for the splash screen
bool Menu::UpdateSplash(float frame_time) 
{
	if (end_splash_ == true || splash_counter_ > 5.0f)
	{
		return true;
	}

	splash_counter_ += frame_time;

	return false;
}

// Updates the various menu timers
bool Menu::UpdateMenu(float frame_time, GameState state)
{
	// If the user presses a button, wait button_delay seconds before they can press another
	if (can_press_button == false)
	{
		button_delay_timer += frame_time;

		if (button_delay_timer > button_delay)
		{
			can_press_button = true;
			button_delay_timer = 0.0f;
		}
	}

	// Updates the timers until they reach 5s
	if (menu_state == WIN)
	{
		win_timer += frame_time;
		if (win_timer > 5.0f)
		{
			menu_state = MAIN;
			end_menu_ = true;
		}
	}
	else if (state == PLAY)
	{
		timer += frame_time;
		if (timer > 5.0f)
		{
			start = false;
		}
	}

	return end_menu_;
}

// Renders the splash screen
void Menu::RenderSplash(gef::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->Begin(false);

	sprite_renderer_->DrawSprite(splash_sprite_);

	sprite_renderer_->End();
}

// Renders the various menu sprites depending the game state
void Menu::RenderMenu(gef::SpriteRenderer* sprite_renderer_, GameState state)
{
	sprite_renderer_->Begin(false);

	switch (menu_state)
	{
	case PRESS_X:
		sprite_renderer_->DrawSprite(menu_sprite_);
		break;
	case MAIN:
	{
		sprite_renderer_->DrawSprite(menu_back_sprite_);

		switch (menu_choice)
		{
		case 0: // start
			select_sprite_.set_position(350.0f, 250.0f, 0.0f);
			break;

		case 1: // options
			select_sprite_.set_position(350.0f, 330.0f, 0.0f);
			break;

		case 2: // exit
			select_sprite_.set_position(350.0f, 410.0f, 0.0f);
			break;
		}

		sprite_renderer_->DrawSprite(select_sprite_);

		break; 
	}
	case TUTORIAL:
	{
		switch (tutorial_choice)
		{
		case 0:
			sprite_renderer_->DrawSprite(tutorial_1_sprite_);
			break;
		case 1:
			sprite_renderer_->DrawSprite(tutorial_2_sprite_);
			break;
		case 2:
			sprite_renderer_->DrawSprite(tutorial_3_sprite_);
			break;
		default:
			break;
		}

		break; 
	}
	case PAUSE:
		RenderMiniMenu(sprite_renderer_, pause_choice, pause_sprite_);
		break;
	case QUIT:
		RenderMiniMenu(sprite_renderer_, quit_choice, quit_sprite_);
		break;
	case LOADING:
		sprite_renderer_->DrawSprite(loading_sprite_);
		break;
	case WIN:
		RenderEnd(sprite_renderer_);
		break;
	case GAMES:
		if (!markers_found_)
		{
			RenderWarning(sprite_renderer_, state);
		}
		else
		{
			if (render_help)
			{
				RenderHelp(sprite_renderer_, state);
			}
		}
		break;
	default:
		break;
	}
	
	sprite_renderer_->End();
}

// Renders a menu where the user has two options
void Menu::RenderMiniMenu(gef::SpriteRenderer* sprite_renderer_, bool choice, gef::Sprite sprite)
{
	sprite_renderer_->DrawSprite(sprite);


	if (paused_)
	{
		select_sprite_.set_height(22.0f / 544.0f * 2.0f);
		select_sprite_.set_width(22.0f / 960.0f * 2.0f);
	}
	else
	{
		select_sprite_.set_height(22.0f);
		select_sprite_.set_width(22.0f);
	}

	// Sets the position of the select sprite depening on the current choice
	switch (choice)
	{
	case true: // Top option
	{
		if (paused_)
		{
			select_sprite_.set_position(-0.25, -0.07, 0.0f);
		}
		else
		{
			select_sprite_.set_position(350.0f, 250.0f, 0.0f);
		}
		break;
	}

	case false: // Bottom option
	{
		if (paused_)
		{
			select_sprite_.set_position(-0.25, 0.22, 0.0f);
		}
		else
		{
			select_sprite_.set_position(350.0f, 330.0f, 0.0f);
		}

		break;
	}
	}

	sprite_renderer_->DrawSprite(select_sprite_);

}

// Renders the loading screen
void Menu::RenderLoading(gef::Platform& platform_, gef::SpriteRenderer* sprite_renderer_)
{
	sprite_renderer_->Begin(false);
	platform_.PreRender();
	sprite_renderer_->DrawSprite(loading_sprite_);
	platform_.PostRender();
	sprite_renderer_->End();
}

// Renders the sprite telling the user who won
void Menu::RenderEnd(gef::SpriteRenderer* sprite_renderer_)
{
	if (winner == 0)
	{
		sprite_renderer_->DrawSprite(y_win_sprite_);	
	}
	else if(winner == 1)
	{	
		sprite_renderer_->DrawSprite(r_win_sprite_);	
	}
}

// Renders the sprite telling the user that the vita cant see the correct amount of markers
void Menu::RenderWarning(gef::SpriteRenderer* sprite_renderer_, GameState state)
{
	if (state == MAKE_TABLE)
	{
		sprite_renderer_->DrawSprite(marker1_sprite_);
	}
	else if (state == PLAY)
	{
		sprite_renderer_->DrawSprite(marker2_sprite_);
	}
}

// Renders the various ingame help sprites
void Menu::RenderHelp(gef::SpriteRenderer* sprite_renderer_, GameState state)
{
	if (state == MAKE_TABLE)
	{
		sprite_renderer_->DrawSprite(lock_table_);
	}
	else if (state == PLAY)
	{
		if (start)
		{
			sprite_renderer_->DrawSprite(this_cue_);
		}
		else
		{
			sprite_renderer_->DrawSprite(get_cue_back_);
		}
	}
}

// Updates if the correct amount of markers can be seen
void Menu::UpdateMarkerFound(bool found)
{
	markers_found_ = found;
}

// Tells the menu that the game has been won, and who has won
void Menu::UpdateWinner(int winner)
{
	this->winner = winner;
	menu_state = WIN;
	winner_found_ = true;
}

// Returns if the game is paused (true) or not (false)
bool Menu::IsPaused()
{
	return paused_;
}

// Ends the splash screen
void Menu::EndSplash()
{
	end_splash_ = true;
}

// Updates the menu depending on user input and what state the game is in
void Menu::ButtonPressed(UInt32 button_num, State state)
{
	if (can_press_button)
	{
		if (state != GAME)
		{
			switch (button_num)
			{
			case (gef_SONY_CTRL_CROSS):
			{
				if (state == SPLASH)
				{
					EndSplash();
				}
				else if(state == MENU)
				{
					switch (menu_state)
					{
					case PRESS_X:
						menu_state = MAIN;
						break;
					case MAIN:
					{
						// chooses an menu option
						switch (menu_choice)
						{
						case 0: // start
							end_menu_ = true;
							break;

						case 1: // options
							in_tutorial_ = true;
							tutorial_choice = 0;
							menu_state = TUTORIAL;
							break;
						case 2: // exit
							menu_state = QUIT;
							break;
						}
						break;
					}			
					case QUIT:
					{
						switch (quit_choice)
						{
						case 0:
							menu_state = MAIN;
							break;
						case 1:
							exit(0);
							break;
						default:
							break;
						}
						break;
					}
					default:
						break;
					}

				}
				

				can_press_button = false;
				break;
			}
			case (gef_SONY_CTRL_DOWN):
			{
				switch (menu_state)
				{				
				case MAIN:
					menu_choice++;
					if (menu_choice > 2)
					{
						menu_choice = 0;
					}
					break;				
				case QUIT:
					quit_choice = !quit_choice;
					break;
				default:
					break;
				}

				can_press_button = false;

				break;
			}
			case (gef_SONY_CTRL_UP):
			{
				switch (menu_state)
				{
				case MAIN:
					menu_choice--;
					if (menu_choice < 0)
					{
						menu_choice = 2;
					}
					break;
				case PAUSE:
					pause_choice = !pause_choice;
					break;
				case QUIT:
					quit_choice = !quit_choice;
					break;
				default:
					break;
				}
				
				can_press_button = false;
				break;
			}
			case(gef_SONY_CTRL_LEFT):
			{
				if (menu_state == TUTORIAL)
				{
					tutorial_choice--;
					if (tutorial_choice < 0)
					{
						in_tutorial_ = false;
						menu_state = MAIN;

					}
					can_press_button = false;
				}

				break;
			}
			case(gef_SONY_CTRL_RIGHT):
			{
				if (menu_state == TUTORIAL)
				{
					tutorial_choice++;
					if (tutorial_choice > 2)
					{
						tutorial_choice = 2;
					}
					can_press_button = false;
				}

				break;
			}
			default:
				break;
			}
		}
		else
		{			
			switch (button_num)
			{
			case (gef_SONY_CTRL_UP):
			case (gef_SONY_CTRL_DOWN):
			{
				if (menu_state == PAUSE)
				{
					pause_choice = !pause_choice;
				}
				break;
			}
			case (gef_SONY_CTRL_CROSS):
			{
				if (menu_state == PAUSE)
				{
					switch (pause_choice)
					{
					case true:
						paused_ = false;
						menu_state = GAMES;
						break;
					case false:
						menu_state = MAIN;
						end_menu_ = true;
						break;
					default:
						break;
					}
				}
				break;
			}
			case (gef_SONY_CTRL_START):
			{
				paused_ = !paused_;

				if (menu_state != PAUSE)
				{
					menu_state = PAUSE;
				}
				else
				{
					menu_state = GAMES;
				}
			}
			case (gef_SONY_CTRL_CIRCLE):
			{
				render_help = false;
			}
			default:
				break;
			}			
				
		}	
	}
}

// updates the menu that the game has begun
void Menu::Playing()
{
	menu_state = GAMES;
}

// Loads in a .png file and returns it as a texture
gef::Texture* Menu::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
{
	gef::PNGLoader png_loader;
	gef::ImageData image_data;
	gef::Texture* texture = NULL;

	// load image data from PNG file 
	png_loader.Load(png_filename, platform, image_data);

	// if the image data is valid, then create a texture from it
	if (image_data.image() != NULL)
		texture = gef::Texture::Create(platform, image_data);

	return texture;
}
