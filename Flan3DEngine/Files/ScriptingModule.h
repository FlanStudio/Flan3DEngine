#ifndef __SCRIPTINGMODULE_H__
#define __SCRIPTINGMODULE_H__

#include "Module.h"

#include <string>
#include <vector>
#include <map>

class ComponentScript;
struct _MonoDomain;
struct _MonoAssembly;
class ResourceScript;
struct _MonoObject;
struct _MonoImage;

bool exec(const char* cmd, std::string& error = std::string());
_MonoObject* InstantiateGameObject();

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
	ComponentScript* CreateScriptComponent(std::string scriptName, bool createCS = true);
	bool DestroyScript(ComponentScript* script);

	bool alreadyCreated(std::string scriptName);

	void CreateScriptingProject();
	void ExecuteScriptingProject();
	void IncludecsFiles();

	void CreateInternalCSProject();

	std::string getReferencePath()const;
	
	std::string clearSpaces(std::string& scriptName = std::string());

	void CreateDomain();
	void ReInstance();

	void GameObjectChanged(GameObject* gameObject);
	void MonoObjectChanged(_MonoObject* monoObject);

public:
	_MonoDomain* domain = nullptr;
	_MonoAssembly* internalAssembly = nullptr;
	_MonoImage* internalImage = nullptr;

	//The relationship between the actual GameObjects and their CSharp representation
	std::vector<std::pair<GameObject*, _MonoObject*>> gameObjectsMap;

private:
	std::vector<ComponentScript*> scripts;
};

#endif