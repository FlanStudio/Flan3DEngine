#pragma once

#include "Globals.h"
#include "Parson/parson.h"

class Application;

class Module
{
private :
	bool enabled;
	char* name;

public:
	Module(char* name, bool start_enabled = true) : name(name), enabled(start_enabled) {}


	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	const char* getName() { return name; }

	bool isEnabled() const { return enabled; }

	bool Enable()
	{
		if (!enabled)
			Start();
		enabled = true;
	}

	bool Disable()
	{
		if (enabled)
			CleanUp();
		enabled = false;
	}

	//Save changes the JSON, not the module
	virtual bool Save(JSON_Object* obj) const { return true; }

	//Load changes the module, not the JSON
	virtual bool Load(const JSON_Object* obj) { return true; }



};