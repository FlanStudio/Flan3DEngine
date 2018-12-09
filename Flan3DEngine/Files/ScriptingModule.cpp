#include "ScriptingModule.h"
#include "ComponentScript.h"
#include "ResourceScript.h"

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>

#include <array>

bool exec(const char* cmd, std::string& error)
{
	std::array<char, 128> buffer;
	bool result;
	auto pipe = _popen(cmd, "r");

	if (!pipe) throw std::runtime_error("popen() failed!");

	while (!feof(pipe))
	{
		if (fgets(buffer.data(), 128, pipe) != nullptr)
			error += buffer.data();
	}

	auto rc = _pclose(pipe);

	if (rc == EXIT_SUCCESS)
	{
		std::cout << "SUCCESS\n";
		result = true;
	}
	else
	{
		std::cout << "FAILED\n";
		result = false;
	}

	return result;
}

bool ScriptingModule::Init()
{
	//Locate the lib and etc folders in the mono installation
	std::string MonoLib, MonoEtc, gamePath = App->fs->getAppPath();
	MonoLib = gamePath + "Mono\\lib";
	MonoEtc = gamePath + "Mono\\etc";

	mono_set_dirs(MonoLib.data(), MonoEtc.data());
	mono_config_parse(NULL);

	//Initialize the mono domain
	domain = mono_jit_init("Scripting");
	if (!domain)
		return false;

	CreateDomain();

	char* args[1];
	args[0] == "InternalAssembly";
	mono_jit_exec(domain, internalAssembly, 1, args);

	if (!internalAssembly)
		return false;

	return true;
}

bool ScriptingModule::Start()
{
	CreateScriptingProject();
	IncludeCSFiles();
	LoadResources();
	return true;
}

update_status ScriptingModule::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		CreateInternalCSProject();

	//Update Time.deltaTime and Time.realDeltaTime values
	mono_field_static_set_value(timeVTable, deltaTime, &App->time->playDt);
	mono_field_static_set_value(timeVTable, realDeltaTime, &App->time->dt);
	mono_field_static_set_value(timeVTable, time, &App->time->gameTime);
	mono_field_static_set_value(timeVTable, realTime, &App->time->timer);

	return UPDATE_CONTINUE;
}

update_status ScriptingModule::Update()
{
	for (int i = 0; i < gameObjectsMap.size(); ++i)
	{
		GameObjectChanged(gameObjectsMap[i].first);
	}
	
	for (int i = 0; i < scripts.size(); ++i)
	{	
		scripts[i]->Update();
	}

	for (int i = 0; i < gameObjectsMap.size(); ++i)
	{
		MonoObjectChanged(gameObjectsMap[i].second);
	}

	//TODO: PREUPDATE AND POSTUPDATE FOR LOOPS WILL BE IN THIS UPDATE TOO.

	return UPDATE_CONTINUE;
}

update_status ScriptingModule::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ScriptingModule::CleanUp()
{
	for (int i = 0; i < scripts.size(); ++i)
	{
		delete scripts[i];
	}
	scripts.clear();

	mono_jit_cleanup(domain);
	domain = nullptr;

	return true;
}

void ScriptingModule::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::PLAY:
		{
			//Check if some files have compile errors and don't let the user hit the play.

			//Call the Awake and Start for all the Enabled script in the Play instant.
			break;
		}
		case EventType::PAUSE:
		{
			//Stop calling the PreUpdate, Update, PostUpdate methods in the active ComponentScripts.
			break;
		}
		case EventType::STOP:
		{
			//Call the CleanUp method for all the scripts which where enabled at this point.
			break;
		}

		case EventType::RESOURCE_DESTROYED:
		{		
			for (int i = 0; i < scripts.size(); ++i)
			{
				bool somethingDestroyed = false;
				if (scripts[i]->scriptRes == event.resEvent.resource)
				{
					somethingDestroyed = true;
					scripts[i]->gameObject->ClearComponent(scripts[i]);
					delete scripts[i];
					scripts.erase(scripts.begin() + i);

					i--;
				}
				if (somethingDestroyed)
				{
					IncludeCSFiles();
				}
			}			
		}

		case EventType::GO_DESTROYED:
		{
			for (int i = 0; i < gameObjectsMap.size(); ++i)
			{
				if (gameObjectsMap[i].first == event.goEvent.gameObject)
				{
					MonoClass* monoObjectClass = mono_object_get_class(gameObjectsMap[i].second);
					MonoClassField* deletedField = mono_class_get_field_from_name(monoObjectClass, "destroyed");

					bool temp = true;
					mono_field_set_value(gameObjectsMap[i].second, deletedField, &temp);

					gameObjectsMap.erase(gameObjectsMap.begin() + i);
					i--;
				}
			}			
		}

		//TODO: Create and receive the ComponentEnabled event, check if the component is an script, Awake him during runtime and start calling the Update's methods.
	}
}

ComponentScript* ScriptingModule::CreateScriptComponent(std::string scriptName, bool createCS)
{
	while (scriptName.find(" ") != std::string::npos)
	{
		scriptName = scriptName.replace(scriptName.find(" "), 1, "");
	}

	ComponentScript* script = new ComponentScript(scriptName);
	char* buffer;
	int size;

	if (createCS)
	{
		App->fs->OpenRead("Internal/SampleScript/SampleScript.cs", &buffer, size);

		std::string scriptStream = buffer;
		scriptStream.resize(size);

		while (scriptStream.find("SampleScript") != std::string::npos)
		{
			scriptStream = scriptStream.replace(scriptStream.find("SampleScript"), 12, scriptName);
		}

		App->fs->OpenWriteBuffer("Assets/Scripts/" + scriptName + ".cs", (char*)scriptStream.c_str(), scriptStream.size());

		IncludeCSFiles();

		delete buffer;
	}

	ResourceScript* scriptRes = (ResourceScript*)App->resources->FindByFile("Assets/Scripts/" + scriptName + ".cs");
	if (!scriptRes)
	{
		//Here we have to reference a new ResourceScript with the .cs we have created, but the ResourceManager will still be sending file created events, and we would have data duplication.
		//We disable this behavior and control the script creation only with this method, so we do not care for external files out-of-engine created.
		scriptRes = new ResourceScript();
		scriptRes->setFile("Assets/Scripts/" + scriptName + ".cs");
		scriptRes->scriptName = scriptName;

		//Create the .meta, to make faster the search in the map storing the uid.
		uint bytes = sizeof(UID);
		char* buffer = new char[bytes];
		UID uid = scriptRes->getUUID();
		memcpy(buffer, &uid, bytes);

		App->fs->OpenWriteBuffer("Assets/Scripts/" + scriptName + ".cs.meta", buffer, bytes);

		delete buffer;

		scriptRes->Compile();
		App->resources->PushResourceScript(scriptRes);
	}

	script->scriptRes = scriptRes;

	scripts.push_back(script);

	return script;
}

bool ScriptingModule::DestroyScript(ComponentScript* script)
{
	for (int i = 0; i < scripts.size(); ++i)
	{
		if (scripts[i] == script)
		{
			delete script;
			scripts.erase(scripts.begin() + i);
			return true;
		}
	}

	return false;
}

bool ScriptingModule::alreadyCreated(std::string scriptName)
{
	clearSpaces(scriptName);

	for (int i = 0; i < scripts.size(); ++i)
	{
		if (scriptName == scripts[i]->scriptName)
			return true;
	}

	return false;
}

void ScriptingModule::LoadResources()
{
	Directory scriptsDir = App->fs->getDirFiles("Assets/Scripts");
	LoadResources(scriptsDir);
}

void ScriptingModule::LoadResources(const Directory& dir)
{
	for (int i = 0; i < dir.files.size(); ++i)
	{
		if (App->fs->getExt(dir.files[i].name) != ".cs")
			continue;

		std::string scriptName = dir.files[i].name;
		scriptName = scriptName.substr(0, scriptName.find_last_of("."));

		std::string filePath = dir.fullPath + dir.files[i].name;

		ResourceScript* scriptRes = new ResourceScript();
		scriptRes->setFile(filePath);
		scriptRes->scriptName = scriptName;

		uint bytes = sizeof(UID);
		char* buffer = new char[bytes];
		UID uid = scriptRes->getUUID();
		memcpy(buffer, &uid, bytes);

		App->fs->OpenWriteBuffer(filePath + ".meta", buffer, bytes);

		delete buffer;

		scriptRes->Compile();

		App->resources->PushResourceScript(scriptRes);
	}

	for (int i = 0; i < dir.directories.size(); ++i)
	{
		LoadResources(dir.directories[i]);
	}
}

void ScriptingModule::CreateScriptingProject()
{
	if (App->fs->Exists("ScriptingProject.sln"))
		return;

	App->fs->CopyDirectoryAndContentsInto("Internal/ScriptingProject", "", false);
}

void ScriptingModule::ExecuteScriptingProject()
{
	//We need the path to the Visual Studio .exe and after that ask the program to start opening the right solution. No idea how to do that for now.
#if 0
	CreateScriptingProject();

	IncludecsFiles();

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	bool ret = CreateProcess("C:\\Users\\Jony635\\Desktop\\Proyectos 3o\\GitHub\\Flan3DEngine\\Flan3DEngine\\Game\\ScriptingProject.sln", "C:\\Users\\Jony635\\Desktop\\Proyectos 3o\\GitHub\\Flan3DEngine\\Flan3DEngine\\Game\\ScriptingProject.sln", 0, 0, false, 0, 0, 0, &si, &pi) != 0;

	if (!ret)
	{
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		Debug.LogError("Could not open \"ScriptingProject.sln\". Error: %s", message.data());

		//Free the buffer.
		LocalFree(messageBuffer);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
#endif
}

void ScriptingModule::IncludeCSFiles()
{
	Directory scripts = App->fs->getDirFiles("Assets/Scripts");

	//Modify the project settings file, stored in a xml

	char* buffer;
	int size;
	if (!App->fs->OpenRead("Assembly-CSharp.csproj", &buffer, size))
		return;

	pugi::xml_document configFile;
	configFile.load_buffer(buffer, size);
	pugi::xml_node projectNode = configFile.child("Project");
	pugi::xml_node itemGroup = projectNode.child("ItemGroup").next_sibling();
	
	for (pugi::xml_node compile = itemGroup.child("Compile"); compile != nullptr;)
	{
		pugi::xml_node next = compile.next_sibling();

		//Do not delete the internal files
		if (std::string(compile.attribute("Include").as_string()).find("internal") != std::string::npos)
		{
			compile = next;
			continue;
		}
			
		itemGroup.remove_child(compile);
		compile = next;
	}

	IncludeCSFiles(itemGroup, scripts);

	std::ostringstream stream;
	configFile.save(stream, "\r\n");
	App->fs->OpenWriteBuffer("Assembly-CSharp.csproj", (char*)stream.str().data(), stream.str().size());	
}

void ScriptingModule::IncludeCSFiles(pugi::xml_node& nodeToAppend, const Directory& dir)
{
	for (int i = 0; i < dir.files.size(); ++i)
	{
		if (App->fs->getExt(dir.files[i].name) != ".cs")
			continue;

		nodeToAppend.append_child("Compile").append_attribute("Include").set_value(std::string("Assets\\Scripts\\" + dir.files[i].name).data());
	}

	for (int i = 0; i < dir.directories.size(); ++i)
	{
		IncludeCSFiles(nodeToAppend, dir.directories[i]);
	}
}

void ScriptingModule::CreateInternalCSProject()
{
	if (App->fs->Exists("FlanCS"))
		return;

	App->fs->CopyDirectoryAndContentsInto("Internal/FlanCS", "", true);
}

std::string ScriptingModule::getReferencePath() const
{
	return std::string("-r:") + std::string("\"") + App->fs->getAppPath() + std::string("FlanCS.dll\" ");
}

std::string ScriptingModule::clearSpaces(std::string& scriptName)
{
	while (scriptName.find(" ") != std::string::npos)
	{
		scriptName = scriptName.replace(scriptName.find(" "), 1, "");
	}
	return scriptName;
}

void ScriptingModule::ReInstance()
{
	gameObjectsMap.clear();

	for (int i = 0; i < scripts.size(); ++i)
	{	
		scripts[i]->InstanceClass();
	}
}

void ScriptingModule::GameObjectChanged(GameObject* gameObject)
{
	MonoObject* monoObject = nullptr;
	//Find for a pair coincidence in the map
	for (int i = 0; i < gameObjectsMap.size(); ++i)
	{
		if (gameObjectsMap[i].first == gameObject)
		{
			monoObject = gameObjectsMap[i].second;

			//SetUp all the GameObject* fields to the MonoObject

			MonoClass* gameObjectClass = mono_class_from_name(App->scripting->internalImage, "FlanEngine", "GameObject");

			//SetUp the name
			MonoClassField* name = mono_class_get_field_from_name(gameObjectClass, "name");
			MonoString* nameCS = mono_string_new(App->scripting->domain, gameObject->name.data());
			mono_field_set_value(monoObject, name, (void*)nameCS);

			//TODO: CONTINUE UPDATING THINGS
		}
	}

}

void ScriptingModule::MonoObjectChanged(_MonoObject* monoObject)
{
	//Find for a coincidence in the map.

	GameObject* gameObject = nullptr;
	for (int i = 0; i < gameObjectsMap.size(); ++i)
	{
		if (gameObjectsMap[i].second == monoObject)
		{
			gameObject = gameObjectsMap[i].first;

			//SetUp the name
			MonoClass* gameObjectClass = mono_class_from_name(internalImage, "FlanEngine", "GameObject");
			MonoClassField* nameCS = mono_class_get_field_from_name(gameObjectClass, "name");
			MonoString* name = (MonoString*)mono_field_get_value_object(domain, nameCS, monoObject);
			char* newName = mono_string_to_utf8(name);
			gameObject->name = newName;

			//TODO: CONTINUE UPDATING THINGS
		}
	}
}

void DebugLogTranslator(MonoString* msg)
{
	MonoError error;
	char* string = mono_string_to_utf8_checked(msg, &error);

	if (!mono_error_ok(&error))
		return;

	Debug.Log(string);

	mono_free(string);
}

void DebugLogWarningTranslator(MonoString* msg)
{
	MonoError error;
	char* string = mono_string_to_utf8_checked(msg, &error);

	if (!mono_error_ok(&error))
		return;

	Debug.LogWarning(string);

	mono_free(string);
}

void DebugLogErrorTranslator(MonoString* msg)
{
	MonoError error;
	char* string = mono_string_to_utf8_checked(msg, &error);

	if (!mono_error_ok(&error))
		return;

	Debug.LogError(string);

	mono_free(string);
}

void ClearConsole() { Debug.Clear(); }

int32_t GetKeyStateCS(int32_t key)
{
	return App->input->GetKey(key);
}

_MonoObject* InstantiateGameObject()
{
	GameObject* instance = App->scene->CreateGameObject(App->scene->getRootNode());

	MonoClass* gameObjectClass = mono_class_from_name(App->scripting->internalImage, "FlanEngine", "GameObject");
	MonoObject* monoInstance = mono_object_new(App->scripting->domain, gameObjectClass);
	mono_runtime_object_init(monoInstance);

	App->scripting->gameObjectsMap.push_back(std::pair<GameObject*, _MonoObject*>(instance, monoInstance));
	
	App->scripting->GameObjectChanged(instance);

	return monoInstance;
}

void DestroyObj(MonoObject* obj)
{
	bool found = false;
	for (int i = 0; i < App->scripting->gameObjectsMap.size(); ++i)
	{
		if (obj == App->scripting->gameObjectsMap[i].second)
		{
			found = true;

			MonoClass* monoClass = mono_object_get_class(obj);
			MonoClassField* destroyed = mono_class_get_field_from_name(monoClass, "destroyed");
			mono_field_set_value(obj, destroyed, &found);

			GameObject* toDelete = App->scripting->gameObjectsMap[i].first;

			App->scripting->gameObjectsMap.erase(App->scripting->gameObjectsMap.begin() + i);

			App->scene->DestroyGameObject(toDelete);

			break;
		}
	}
}

void ScriptingModule::CreateDomain()
{
	static bool firstDomain = true;

	MonoDomain* nextDom = mono_domain_create_appdomain("The reloaded domain", NULL);
	if (!nextDom)
		return;

	if (!mono_domain_set(nextDom, false))
		return;
	
	//Make sure we do not delete the main domain
	if (!firstDomain)
		mono_domain_unload(domain);

	domain = nextDom;

	char* buffer;
	int size;
	if (!App->fs->OpenRead("FlanCS.dll", &buffer, size, false))
		return;

	//Loading assemblies from data instead of from file
	MonoImageOpenStatus status = MONO_IMAGE_ERROR_ERRNO;
	internalImage = mono_image_open_from_data(buffer, size, 1, &status);
	internalAssembly = mono_assembly_load_from(internalImage, "InternalAssembly", &status);

	delete buffer;

	timeClass = mono_class_from_name(internalImage, "FlanEngine", "Time");
	deltaTime = mono_class_get_field_from_name(timeClass, "deltaTime");
	realDeltaTime = mono_class_get_field_from_name(timeClass, "realDeltaTime");
	time = mono_class_get_field_from_name(timeClass, "time");
	realTime = mono_class_get_field_from_name(timeClass, "realTime");
	timeVTable = mono_class_vtable(domain, timeClass);

	//SetUp Internal Calls
	mono_add_internal_call("FlanEngine.Debug::Log", (const void*)&DebugLogTranslator);
	mono_add_internal_call("FlanEngine.Debug::LogWarning", (const void*)&DebugLogWarningTranslator);
	mono_add_internal_call("FlanEngine.Debug::LogError", (const void*)&DebugLogErrorTranslator);
	mono_add_internal_call("FlanEngine.Debug::ClearConsole", (const void*)&ClearConsole);
	mono_add_internal_call("FlanEngine.GameObject::Instantiate", (const void*)&InstantiateGameObject);
	mono_add_internal_call("FlanEngine.Input::GetKeyState", (const void*)&GetKeyStateCS);
	mono_add_internal_call("FlanEngine.Object::Destroy", (const void*)&DestroyObj);

	firstDomain = false;
}
