#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include <string>
#include "Parson/parson.h"


class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
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
	bool showConfig = false;
	bool showAbout = false;
	bool propWindow = false;

	std::string nameJSON;
};