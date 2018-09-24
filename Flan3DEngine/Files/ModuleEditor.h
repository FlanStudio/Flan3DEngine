#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "imgui/tabs/imgui_tabs.h"
#include <string>

struct LogWindow
{
private:
	std::string logsTitle;
	std::string warningTitle;
	std::string errorTitle;
	int numLogs=0;
	int numwarnings = 0;
	int numerrors = 0;

	ImGuiTextBuffer     NormalBuf;
	ImGuiTextBuffer     WarningBuf;
	ImGuiTextBuffer     ErrorBuf;

public:
	bool                ScrollToBottom = true;
	void    Clear() { NormalBuf.clear(); WarningBuf.clear(); ErrorBuf.clear(); numLogs = 0; numwarnings = 0; numerrors = 0; }
	void    Log(const char* format, ...);
	void    LogWarning(const char* format, ...);
	void    LogError(const char* format, ...);
	void Draw(const char* title, bool* p_opened = NULL);

};

class ModuleEditor : public Module
{
public:
	LogWindow logWindow;
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	virtual ~ModuleEditor();

	bool Init();

	bool Start();

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	update_status PostUpdate(float dt);

	bool CleanUp();

	
private:
	bool showdemowindow = false;
	bool showMGLwindow = false;
	bool somethingiscolliding = false;
	bool logEnabled = false;
};