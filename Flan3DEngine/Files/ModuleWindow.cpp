#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module("ModuleWindow", start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	Debug.Log("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Debug.Log("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL;

		//Use OpenGL 3.1
		
		ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) == 0;
		if (ret)
			ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1) == 0;
		if(ret)
			ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0;
		if (ret)
			ret = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0;
		if(ret)
			ret = SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) == 0;
		if(ret)
			ret = SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) == 0;

		if (!ret)
			Debug.LogError("Some window atributes couldn't be loaded!");




		if (WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->engineName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			Debug.Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}
update_status ModuleWindow::Update(float dt)
{
	SDL_SetWindowTitle(window, (App->engineName + "_" + App->organization).c_str());
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	Debug.Log("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(char* title)
{
	SDL_SetWindowTitle(window, title);
}

bool ModuleWindow::setFullScreenWindow(bool boolean)
{
	bool ret = true;
	
	if (WIN_FULLSCREEN == true)
	{
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
		{
			Debug.Log("Could not switch to fullscreen: %s\n", SDL_GetError());
			ret = false;
		}
		WIN_FULLSCREEN_DESKTOP = false;
	}
	else
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
			Debug.Log("Could not switch to windowed: %s\n", SDL_GetError());
			ret = false;
		}
	}
	
	return ret;
}

bool ModuleWindow::setFullScreenDesktop(bool boolean)
{
	bool ret = true;
	
	if (WIN_FULLSCREEN_DESKTOP == true)
	{
		if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
		{
			Debug.Log("Could not switch to fullscreen: %s\n", SDL_GetError());
			ret = false;
		}
		WIN_FULLSCREEN = false;
	}
	else
	{
		if (SDL_SetWindowFullscreen(window, 0) != 0)
		{
			Debug.Log("Could not switch to windowed: %s\n", SDL_GetError());
			ret = false;
		}
	}
	
	return ret;
}

bool ModuleWindow::setBrightness(float bright)
{
	BRIGHTNESS = bright;
	return SDL_SetWindowBrightness(window, bright) == 0;
}

bool ModuleWindow::setWindowWidth(int width)
{
	SDL_SetWindowSize(window, width, SCREEN_HEIGHT);
	SCREEN_WIDTH = width;
	return true;
}

bool ModuleWindow::setWindowHeight(int height)
{
	SDL_SetWindowSize(window, SCREEN_WIDTH, height);
	SCREEN_HEIGHT = height;
	return true;
}

bool ModuleWindow::setBorderless(bool boolean)
{
	WIN_BORDERLESS = boolean;
	SDL_SetWindowBordered(window, SDL_bool(!boolean));
	return true;
}

bool ModuleWindow::setResizable(bool boolean)
{
	WIN_RESIZABLE = boolean;
	SDL_SetWindowResizable(window, SDL_bool(boolean));
	return true;
}

int ModuleWindow::getRefreshRate() const
{
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);

	return dm.refresh_rate;
}

bool ModuleWindow::Save(JSON_Object* obj) const
{
	json_object_set_boolean(obj, "FullscreenWindow", WIN_FULLSCREEN);
	json_object_set_boolean(obj, "FullscreenDesktop", WIN_FULLSCREEN_DESKTOP);
	json_object_set_boolean(obj, "Resizable", WIN_RESIZABLE);
	json_object_set_boolean(obj, "Borderless", WIN_BORDERLESS);
	json_object_set_number(obj, "ScreenWidth", SCREEN_WIDTH);
	json_object_set_number(obj, "ScreenHeight", SCREEN_HEIGHT);
	json_object_set_number(obj, "Brightness", BRIGHTNESS);
	return true;
}

bool ModuleWindow::Load(const JSON_Object* obj)
{
	WIN_FULLSCREEN = json_object_get_boolean(obj, "FullscreenWindow");
	WIN_FULLSCREEN_DESKTOP = json_object_get_boolean(obj, "FullscreenDesktop");
	WIN_RESIZABLE = json_object_get_boolean(obj, "Resizable");
	WIN_BORDERLESS = json_object_get_boolean(obj, "Borderless");
	SCREEN_WIDTH = json_object_get_number(obj, "ScreenWidth");
	SCREEN_HEIGHT = json_object_get_number(obj, "ScreenHeight");
	BRIGHTNESS = json_object_get_number(obj, "Brightness");

	setFullScreenWindow(WIN_FULLSCREEN);
	setFullScreenDesktop(WIN_FULLSCREEN_DESKTOP);
	setResizable(WIN_RESIZABLE);
	setBorderless(WIN_BORDERLESS);
	setWindowWidth(SCREEN_WIDTH);
	setWindowHeight(SCREEN_HEIGHT);
	setBrightness(BRIGHTNESS);

	return true;
}

