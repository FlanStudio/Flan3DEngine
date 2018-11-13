#ifndef __MODULE_TIME_H__
#define __MODULE_TIME_H__

#include "Module.h"
#include "Globals.h"
#include "Timer.h"

class ResourceTexture;

class ModuleTime : public Module
{
public:
	ModuleTime(bool start_enabled = true) : Module("TextureLoader", start_enabled) {}
	~ModuleTime() {}

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnTimeGUI();
private:
	void ReceiveEvent(Event event);
public:
	float dt = 0.0f;
	float playDt = 0.0f;
	float gameTime = 0.0f;
	float timeScale = 1.0f; //Must be between 0.0f and 2.0f
	float timer = 0.0f;

	Uint32 frameCount = 0;
	Uint32 lastSecFrames = 0;

	Timer dtTimer;

private:
	Uint32 lastSecFramesCounter = 0;

	uint TimeAtlasID = 0;
	
	bool paused = false;
	bool steped = false;

	int h = 0;//delete
};

#endif //__MODULE_TIME_H__
