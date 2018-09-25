#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
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
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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

		window = SDL_CreateWindow(winTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
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
	SDL_SetWindowTitle(window, (winTitle + "_" + organization).c_str());
	return UPDATE_CONTINUE;
}
// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

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
	winTitle = title;
}

bool ModuleWindow::setFullScreenWindow(bool boolean)
{
	bool ret = true;
	if (boolean != WIN_FULLSCREEN)
	{
		WIN_FULLSCREEN = boolean;
		if (WIN_FULLSCREEN == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
			{
				LOG("Could not switch to fullscreen: %s\n", SDL_GetError());
				ret = false;
			}
			WIN_FULLSCREEN_DESKTOP = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
			{
				LOG("Could not switch to windowed: %s\n", SDL_GetError());
				ret = false;
			}
		}
	}
	return ret;
}

bool ModuleWindow::setFullScreenDesktop(bool boolean)
{
	bool ret = true;
	if (boolean != WIN_FULLSCREEN_DESKTOP)
	{
		WIN_FULLSCREEN_DESKTOP = boolean;
		if (WIN_FULLSCREEN_DESKTOP == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
			{
				LOG("Could not switch to fullscreen: %s\n", SDL_GetError());
				ret = false;
			}
			WIN_FULLSCREEN = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
			{
				LOG("Could not switch to windowed: %s\n", SDL_GetError());
				ret = false;
			}
		}
	}
	return ret;
}

bool ModuleWindow::setResizable(bool boolean)
{
	WIN_RESIZABLE = boolean;
	return true;
}

