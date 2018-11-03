#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "FBXLoader.h"
#include "ModuleTextures.h"

#include "Parson/parson.h"

#include <list>
#include <vector>
#include <string>

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
	bool enter = true;

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
		std::string str(format);
		if(enter)
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

extern LogWindow Debug;


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleFileSystem* fs;
	FBXLoader* fbxLoader;
	ModuleTextures* textures;

private:
	Timer	ms_timer;
	float	dt = 0.0f;
	std::list<Module*> list_modules;
	bool load = true;
	bool save = false;

public:
	std::vector<float>FPS;
	int FPS_index = 0;

	std::vector<float>ms;
	int ms_index = 0;

	int maxFPS = 60;
	std::string engineName = "Flan3DEngine";
	std::string organization = "FlanStudio";

	bool debugDraw = true;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void Load();
	void Save();
	void DebugDraw() const;

private:
	bool LoadNow();
	bool SaveNow() const;
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif