#ifndef __SCRIPTINGMODULE_H__
#define __SCRIPTINGMODULE_H__

#include "Module.h"

#include <string>
#include <vector>

#define MONO_LIB "C:\\Program Files (x86)\\Mono\\lib"
#define MONO_ETC "C:\\Program Files (x86)\\Mono\\etc"

class ComponentScript;
struct _MonoDomain;
struct _MonoAssembly;

bool exec(const char* cmd, std::string& error = std::string());

class ScriptingModule : public Module
{
public:
	ScriptingModule(bool start_enabled = true) : Module("ScriptingModule", start_enabled){}
	~ScriptingModule() {}

	bool Init();
	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void ReceiveEvent(Event event);

public:
	ComponentScript* CreateScript(std::string scriptName, bool createCS = true);
	bool DestroyScript(ComponentScript* script);

	bool alreadyCreated(std::string scriptName);

	void CreateScriptingProject();
	void ExecuteScriptingProject();
	void IncludecsFiles();

	void CreateInternalCSProject();

	std::string getReferencePath()const;
	
	std::string clearSpaces(std::string& scriptName = std::string());

public:
	_MonoDomain* domain = nullptr;
	_MonoAssembly* internalAssembly = nullptr;

private:
	std::vector<ComponentScript*> scripts;
};

#endif