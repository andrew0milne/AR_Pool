#include "ar_app.h"
#include <system/platform.h>

#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/image_data.h>
#include <graphics/font.h>

#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <input/sony_controller_input_manager.h>

#include <maths/math_utils.h>
#include <maths/vector2.h>


#include <assets/png_loader.h>

#include <sony_sample_framework.h>
#include <sony_tracking.h>

#include <system\debug_log.h>

static const char* key_names[] =
{
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"NUMPADENTER",
	"NUMPADSTAR",
	"NUMPADEQUALS",
	"NUMPADMINUS",
	"NUMPADPLUS",
	"NUMPADPERIOD",
	"NUMPADSLASH",
	"ESCAPE",
	"TAB",
	"LSHIFT",
	"RSHIFT",
	"LCONTROL",
	"RCONTROL",
	"BACKSPACE",
	"RETURN",
	"LALT",
	"SPACE",
	"CAPSLOCK",
	"NUMLOCK",
	"SCROLL",
	"RALT",
	"AT",
	"COLON",
	"UNDERLINE",
	"MINUS",
	"EQUALS",
	"LBRACKET",
	"RBRACKET",
	"SEMICOLON",
	"APOSTROPHE",
	"GRAVE",
	"BACKSLASH",
	"COMMA",
	"PERIOD",
	"SLASH",
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
	"PAGEUP",
	"PAGEDOWN"
};

ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL)
{
}

void ARApp::Init()
{
	input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	primitive_builder_ = new PrimitiveBuilder(platform_);
	camera_image_texture_ = new gef::TextureVita;

	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
	{
		input_manager_->touch_manager()->EnablePanel(0);
	}


	InitFont();
	SetupLights();


	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);
	
	// Initialises the states
	state = SPLASH;
	game_state = MAKE_TABLE;

	// Creates the menu
	menu = new Menu(platform_, sprite_renderer_, 1.0f);

	// Creates the physics controller
	physics = new Physics();

	InitBalls();
	
	y_scaling_factor_ = ((float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT) / ((float)CAMERA_WIDTH / (float)CAMERA_HEIGHT);

	camera_image_sprite_.set_position(gef::Vector4(0.0f, 0.0f, 1.0));
	camera_image_sprite_.set_width(2.0f);
	camera_image_sprite_.set_height(y_scaling_factor_ * 2.0f);
	camera_image_sprite_.set_texture(camera_image_texture_);

	sceCameraSetISO(CAMERA_DEVICE, CAMERA_ATTRIBUTE_ISO);
	sceCameraSetWhiteBalance(CAMERA_DEVICE, CAMERA_ATTRIBUTE_WHITEBALANCE);


	marker_found_ = false;
}

// initialises the balls
void ARApp::InitBalls()
{
	Tag tag = CUE;

	red_score = 0;
	yellow_score = 0;

	// The max score being the number of one type of coloured balls
	max_score_ = (number_of_balls - 2) / 2;

	// Sets up the colours of the balls
	for (int i = 0; i < number_of_balls; i++)
	{
		if (i > 1)
		{
			if (i % 2 == 0)
			{
				tag = YELLOW;
			}
			else
			{
				tag = RED;
			}
		}
		else if (i == 1)
		{
			tag = WHITE;
		}

		// Iniliases the ball
		balls_[i] = new GameObject(platform_, "ball1.scn", tag);
		balls_[i]->SetScale(0.0002);
		balls_[i]->SetPosition(gef::Vector4((0.02 * i) - 0.03, 0.0, 0.0));
		balls_[i]->SetRotation(gef::Vector4(0.0, 0.0, 0.0));
		balls_[i]->SetVelocity(0.0f, gef::DegToRad(20.0f * i));
		balls_[i]->SetAngularZVelocity(0.0f);

		// The cue ball is heavier
		if (i == 0)
		{
			balls_[i]->SetMass(20.0f);
		}
		else
		{
			balls_[i]->SetMass(10.0f);
		}

		balls_[i]->SetActive(true);
	}

}

void ARApp::CleanUp()
{
	delete primitive_builder_;
	primitive_builder_ = NULL;

	smartRelease();
	sampleRelease();

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete table_;
	table_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;
}

// Updates the various objects based on the aplications state
bool ARApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();		
		ProcessControllerInput();	
	}

	AppData* dat = sampleUpdateBegin();

	switch (state)
	{
	case SPLASH:
	{
		if (menu->UpdateSplash(frame_time))
		{
			// The splash screen has ended
			state = MENU;
		}
		break;
	}
	case MENU:
	{
		if (menu->UpdateMenu(frame_time, game_state))
		{
			// User has started the game
			state = GAME;
			game_state = MAKE_TABLE;
			menu->RenderLoading(platform_, sprite_renderer_);
			menu->InitMenu(GAMES);
			table_ = new Table(platform_, primitive_builder_);
			InitBalls();
		}
		break;
	}
	case GAME:
	{
		if (menu->UpdateMenu(frame_time, game_state))
		{
			// User has ended the game
			EndGame(END);

			break;
		}
		
		// If the game is playing
		if (!menu->IsPaused())
		{
			smartUpdate(dat->currentImage);
			
			table_->Update(frame_time);
			switch (game_state)
			{
			case MAKE_TABLE:
			{
				menu->UpdateMarkerFound(table_->more_than_three_markers);
				break;
			}
			case PLAY:
			{
				if (sampleIsMarkerFound(table_->GetLockedMarkerID()))
				{
					// The marker is found
					menu->UpdateMarkerFound(true);

					gef::Matrix44 transform_;
					sampleGetTransform(table_->GetLockedMarkerID(), &transform_);

					balls_[0]->CueUpdate(frame_time, transform_);
				}
				else
				{
					// Marker not found
					menu->UpdateMarkerFound(false);
				}
				
				// If the marker is found, updates the objects
				if (table_->marker_found_)
				{
					// If a ball is active, update it
					for (int i = 1; i < number_of_balls; i++)
					{
						if (balls_[i]->GetActive())
						{
							balls_[i]->Update(frame_time, table_->GetLocalOrigin());
						}
					}

					// If a collision has occured between a ball and the cue, disable the cue
					if (CueCollision())
					{
						balls_[0]->SetActive(false);
					}

					// If all the balls have stopped moving reactivate the cue
					if (!BallsMoving())
					{
						balls_[0]->SetActive(true);
					}

					Collisions();
				}
				break;
			}
			default:
				break;
			}
			break;
		}
	}
	default:
		break;
	}

	sampleUpdateEnd(dat);

	return true;
}

// Render the game based on its current state
void ARApp::Render()
{
	AppData* dat = sampleRenderBegin();

	// REMEMBER AND SET THE PROJECTION MATRIX HERE	
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;
	gef::Matrix44 scale_matrix;

	switch (state)
	{
	case SPLASH:
	{
		menu->RenderSplash(sprite_renderer_);
		break;
	}
	case MENU:
	{
		menu->RenderMenu(sprite_renderer_, game_state);
		break;
	}
	case GAME:
	{
		projection_matrix = platform_.PerspectiveProjectionFov(SCE_SMART_IMAGE_FOV, ((float)CAMERA_WIDTH / (float)CAMERA_HEIGHT), 0.1f, 1000.0f);
		view_matrix.SetIdentity();
		scale_matrix.SetIdentity();
		scale_matrix.Scale(gef::Vector4(1.0f, y_scaling_factor_, 1.0f));

		gef::Matrix44 final_projection_matrix = scale_matrix * projection_matrix;

		gef::Matrix44 proj_matrix2d;

		proj_matrix2d = platform_.OrthographicFrustum(-1, 1, -1, 1, -1, 1);
		sprite_renderer_->set_projection_matrix(proj_matrix2d);

		sprite_renderer_->Begin(true);
		
		// DRAW CAMERA FEED SPRITE HERE
		if (dat->currentImage)
		{
			camera_image_texture_->set_texture(dat->currentImage->tex_yuv);
			sprite_renderer_->DrawSprite(camera_image_sprite_);
		}

		sprite_renderer_->End();


		// SET VIEW AND PROJECTION MATRIX HERE
		renderer_3d_->set_projection_matrix(final_projection_matrix);

		renderer_3d_->set_view_matrix(view_matrix);

		// Begin rendering 3D meshes, don't clear the frame buffer
		renderer_3d_->Begin(false);
	
		
		if (!menu->IsPaused())
		{
			switch (game_state)
			{
			case MAKE_TABLE:
			{
				if (table_->more_than_three_markers)
				{
					table_->Render(renderer_3d_);
				}
				break;
			}
			case SET_UP:
			{

				break;
			}
			case PLAY:
			{
				if (table_->marker_found_)
				{
					table_->Render(renderer_3d_);


					for (int i = 0; i < number_of_balls; i++)
					{
						if (balls_[i]->GetActive())
						{
							balls_[i]->Render(renderer_3d_);
						}
					}
				}
				break;
			}
			default:
				break;
			}
			
		}
		else 
		{
			menu->RenderMenu(sprite_renderer_, game_state);
		}

		break;	

	}
	default:
		break;
	}

	renderer_3d_->End();

	RenderOverlay();

	menu->RenderMenu(sprite_renderer_, game_state);

	sampleRenderEnd();
}

void ARApp::Collisions()
{
	
	for (int i = 1; i < number_of_balls; i++)
	{
		for (int j = i; j < number_of_balls; j++)
		{
			if (i != j && balls_[i]->GetActive() && balls_[j]->GetActive())
			{
				physics->SphereToSphereCollision(balls_[j], balls_[i], false);
			}
		}
	}

	// Collision for the walls and the balls
	for (int i = 1; i < table_->NumberOfWalls(); i++)
	{
		for (int j = 1; j < number_of_balls; j++)
		{
			if (balls_[j]->GetActive())
			{
				physics->SphereToRayCollision(balls_[j], table_->walls_[i], i);
				
				if (physics->SphereToSphereCollision(balls_[j], table_->pockets_[i], true))
				{
					if (j > 1)
					{
						balls_[j]->SetActive(false);
						AddScore(balls_[j]->tag_);
					}
					else if (j == 1)
					{
						balls_[j]->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f));
					}
				}
				
			}
		}
	}

	for (int i = 1; i < number_of_balls; i++)
	{
		if (balls_[i]->GetActive())
		{
			float d = table_->GetDistance(balls_[i]->GetPosition(), gef::Vector4(0.0f, 0.0f, 0.0f));
			
			if (d > table_->max_distance_)
			{
				balls_[i]->SetActive(false);
				AddScore(balls_[i]->tag_);
			}
		}
	}

}

// Updates the score
void ARApp::AddScore(Tag tag)
{
	if (tag == RED)
	{
		red_score++;
		if (red_score >= max_score_)
		{
			menu->UpdateWinner(1);
		}
	}
	else if (tag == YELLOW)
	{
		yellow_score++;
		if (yellow_score >= max_score_)
		{
			menu->UpdateWinner(0);
		}

	}
}

// Checks is the cue ball has hit anything
bool ARApp::CueCollision()
{
	if (balls_[0]->GetActive())
	{
		for (int i = 1; i < number_of_balls; i++)
		{
			if (balls_[i]->GetActive())
			{
				if (physics->SphereToSphereCollision(balls_[i], balls_[0], false))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// Checks if any balls are moving
bool ARApp::BallsMoving()
{
	for (int i = 1; i < number_of_balls; i++)
	{
		if (balls_[i]->GetActive())
		{
			if (balls_[i]->GetSpeed() != 0.0f)
			{
				return true;
			}
		}
	}

	return false;
}

void ARApp::EndGame(Tag tag)
{
	//menu->EndGame(tag);
	
	state = MENU;
	menu->InitMenu(PRESS_X);

	delete table_;
	table_ = NULL;
}

// Checks if a player has won
bool ARApp::CountBalls()
{
	int red = 0;
	int yellow = 0;

	for (int i = 0; i < number_of_balls; i++)
	{
		if (balls_[i]->GetActive())
		{
			if (balls_[i]->tag_ == RED)
			{
				red++;
			}
			else if(balls_[i]->tag_ == YELLOW)
			{
				yellow++;
			}
		}
	}

	if (red == 0)
	{
		EndGame(RED);
	}
	if (yellow == 0)
	{
		EndGame(YELLOW);
	}
}

void ARApp::RenderOverlay()
{
	//
	// render 2d hud on top
	//
	gef::Matrix44 proj_matrix2d;

	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void ARApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void ARApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void ARApp::DrawHUD()
{
	if(font_)
	{
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void ARApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

// Controller input
void ARApp::ProcessControllerInput()
{
	const gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
	if (controller_input)
	{
		const gef::SonyController* controller = controller_input->GetController(0);
		if (controller)
		{
			switch (state)
			{
			case SPLASH:
			case MENU:
			{	
				// tell menu what button has been pressed
				menu->ButtonPressed(controller->buttons_pressed(), state);
				break;
			}
			case GAME:
			{							
				if (!menu->IsPaused())
				{
					if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
					{
						menu->render_help = false;
					}
					
					switch (game_state)
					{
					case MAKE_TABLE:
					{
						if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
						{
							if (table_->more_than_three_markers)
							{
								table_->Lock();

								game_state = PLAY;

								table_lock_ = true;

								menu->render_help = true;
							}
						}

						break;
					}
					case PLAY:
					{
						if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
						{
							balls_[0]->SetActive(true);
						}

						break;
					}


					default:
						break;
					}
				}

				menu->ButtonPressed(controller->buttons_pressed(), state);

				break; 
			}
			default:
				break;
			}

		}
	}
}
