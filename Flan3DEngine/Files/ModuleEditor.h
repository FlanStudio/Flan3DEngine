#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

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
	void customStyle();

private:

	bool showdemowindow = false;
	bool showMGLwindow = false;
	bool somethingiscolliding = false;
	bool logEnabled = true;
	bool showConfig = false;
	bool showAbout = false;
	bool propWindow = false;
	bool hierarchy = true;
	bool inspector = true;
	bool fileSystem = true;
};
#endif