#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	update_status Update();
	bool CleanUp();


public:

	void SetTitle(char* title);
	bool setFullScreenWindow(bool boolean);
	bool setFullScreenDesktop(bool boolean);
	bool setResizable(bool boolean);
	bool setBrightness(float bright);
	bool setWindowWidth(int width);
	bool setWindowHeight(int height);
	bool setBorderless(bool boolean);
	int getRefreshRate() const;

private:
	
	bool Save(JSON_Object* obj) const override;
	bool Load(const JSON_Object* obj);

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__