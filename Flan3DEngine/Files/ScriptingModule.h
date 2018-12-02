#ifndef __SCRIPTINGMODULE_H__
#define __SCRIPTINGMODULE_H__

#include "Module.h"

#include <string>
#include <vector>

class ComponentScript;

class ScriptingModule : public Module
{
public:
	ScriptingModule(bool start_enabled = true) : Module("ScriptingModule", start_enabled){}
	~ScriptingModule() {}

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void ReceiveEvent(Event event);

public:
	ComponentScript* CreateScript(std::string csPath);
	bool DestroyScript(ComponentScript* script);

private:
	std::vector<ComponentScript*> scripts;
};

#endif