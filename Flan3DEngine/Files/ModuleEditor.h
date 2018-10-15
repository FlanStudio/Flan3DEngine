#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
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

	void Draw() const;

private:

	bool showdemowindow = false;
	bool showMGLwindow = false;
	bool somethingiscolliding = false;
	bool logEnabled = true;
	bool showConfig = true;
	bool showAbout = false;
	bool propWindow = true;
};