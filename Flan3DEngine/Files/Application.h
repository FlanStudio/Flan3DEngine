#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleEditor.h"

#include <list>
#include <vector>

struct LogWindow
{
private:
	std::string logsTitle;
	std::string warningTitle;
	std::string errorTitle;
	int numLogs = 0;
	int numwarnings = 0;
	int numerrors = 0;

	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
public:
	ImGuiTextBuffer     NormalBuf;
	ImGuiTextBuffer     WarningBuf;
	ImGuiTextBuffer     ErrorBuf;

public:
	bool    ScrollToBottom = true;
	void    Clear() { NormalBuf.clear(); WarningBuf.clear(); ErrorBuf.clear(); numLogs = 0; numwarnings = 0; numerrors = 0; }
	void    Log(const char* format, ...)
	{
		int oldsize = NormalBuf.size();
		numLogs++;
		va_list args;
		va_start(args, format);
		NormalBuf.appendfv(format, args);
		NormalBuf.appendfv("\n", args);
		va_end(args);
		int newsize = NormalBuf.size();
		for (int i = oldsize; i < newsize; i++)
		{
			if (NormalBuf[i] == '\n')
				LineOffsets.push_back(i);
		}
		ScrollToBottom = true;
	}
	void    LogWarning(const char* format, ...)
	{
		numwarnings++;
		va_list args;
		va_start(args, format);
		WarningBuf.appendfv(format, args);
		WarningBuf.appendfv("\n", args);
		va_end(args);
		ScrollToBottom = true;
	}
	void    LogError(const char* format, ...)
	{
		numerrors++;
		va_list args;
		va_start(args, format);
		ErrorBuf.appendfv(format, args);
		ErrorBuf.appendfv("\n", args);
		va_end(args);
		ScrollToBottom = true;
	}
	void Draw(const char* title, bool* p_opened = NULL);
};

static LogWindow Debug;


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleEditor* editor;

private:
	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;


public:
	std::vector<float>FPS;
	int FPS_index = 0;

	std::vector<float>ms;
	int ms_index = 0;

	int maxFPS = 60;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;