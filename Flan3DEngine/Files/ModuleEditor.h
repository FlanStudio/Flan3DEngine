#pragma once

#include "Module.h"
#include "Globals.h"

class ModuleEditor : public Module
{
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
	
};