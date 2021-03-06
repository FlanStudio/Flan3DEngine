#include "Application.h"
#include "Brofiler\Brofiler.h"
#include "GameObject.h"

LogWindow Debug;

void Application::SendEvent(Event event)
{
	events.push(event);
}

void Application::SendEvents()
{
	BROFILER_CATEGORY("Send Events", Profiler::Color::Red);
	while (events.size() > 0)
	{
		Event event = events.front();

		std::list<Module*>::iterator it;
		for (it = list_modules.begin(); it != list_modules.end(); ++it)
		{
			(*it)->ReceiveEvent(event);
		}

		//Deallocate memory
		switch (event.type)
		{
			case EventType::FILE_CREATED:
			case EventType::FILE_DELETED:
			{
				if (event.fileEvent.file)
					delete event.fileEvent.file;
				break;
			}
			case EventType::FILE_MOVED:
			{
				if (event.fileEvent.file)
					delete event.fileEvent.file;

				if (event.fileEvent.oldLocation)
					delete event.fileEvent.oldLocation;
				break;
			}
			case EventType::GO_DESTROYED:
			{
				delete event.goEvent.gameObject;
				break;
			}
		}
		events.pop();
	}
}

Application::Application()
{
	time = new ModuleTime();
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio();
	scene = new ModuleScene();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	editor = new ModuleEditor();
	fs = new ModuleFileSystem();
	fbxexporter = new FBXExporter();
	textures = new ModuleTextures();
	resources = new ResourceManager();
	scripting = new ScriptingModule();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(fs);
	AddModule(textures);
	AddModule(audio);
	AddModule(time);
	AddModule(camera);
	AddModule(fbxexporter);
	AddModule(resources);

	// Scenes
	AddModule(scene);

	//Scripting
	AddModule(scripting);

	//Editor
	AddModule(editor);
	
	// Renderer last!
	AddModule(renderer3D);

	FPS.resize(50);
	ms.resize(50);
}

Application::~Application()
{
	FPS.clear();
	ms.clear();
	std::list <Module*>::reverse_iterator it;
	for (it = list_modules.rbegin(); it != list_modules.rend(); ++it)
	{
		delete (*it);
	}
	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;
	// Call Init() in all modules
	std::list<Module*>::iterator it;
	for (it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	Debug.Log("-------------- Application Start ------------------");
	for (it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		if((*it)->isEnabled())
			ret = (*it)->Start();
	}
	
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	float dt = time->dt;

	float timeFrame = 1.0f / maxFPS;

	dt = (float)time->dtTimer.Read() / 1000.0f;

	if (dt < timeFrame)
	{
		SDL_Delay((timeFrame - dt) * 1000.0f);
	}

	time->dt = (float)time->dtTimer.Read() / 1000.0f;

	FPS[FPS_index++] = time->lastSecFrames;
	if (FPS_index >= FPS.size())
		FPS_index = 0;

	ms[ms_index++] = (time->dt) * 1000;
	if (ms_index >= ms.size())
		ms_index = 0;

	//Save and Load config
	if (save)
	{
		save = false;
		SaveNow();
	}

	if (load)
	{
		load = false;
		LoadNow();
	}
}

void Application::DebugDraw() const
{
	BROFILER_CATEGORY("Debug Draw", Profiler::Color::Yellow)

	App->scene->debugDraw();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	BROFILER_CATEGORY("App_Update", Profiler::Color::Yellow)

	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator it;

	//Send last frame events before doing anything
	SendEvents();
	
	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate();
	}

	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->Update();
	}

	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate();
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	std::list<Module*>::reverse_iterator it;
	for (it = list_modules.rbegin(); it != list_modules.rend() && ret == UPDATE_CONTINUE; ++it)
	{
		if((*it)->isEnabled())
			ret = (*it)->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::Load()
{
	load = true;
}

void Application::Save()
{
	save = true;
}

bool Application::LoadNow()
{
	bool ret = true;

	char* buf;
	int size;
	if (App->fs->Exists(SETTINGS_FOLDER + std::string("settings.json"))) 
	{
		if (fs->OpenRead(SETTINGS_FOLDER + std::string("settings.json"), &buf, size))
		{
			JSON_Value* root = json_parse_string(buf);
			JSON_Object* rootObj = json_value_get_object(root);

			JSON_Object* AppObj = json_object_get_object(rootObj, "Application");
			if (AppObj)
			{
				maxFPS = json_object_get_number(AppObj, "maxFPS");
				engineName = json_object_get_string(AppObj, "engineName");
				organization = json_object_get_string(AppObj, "organization");
			}
			else
				ret = false;

			std::list<Module*>::iterator it;
			for (it = list_modules.begin(); it != list_modules.end() && ret; ++it)
			{
				char* name = (char*)(*it)->getName();

				JSON_Object* itObj = json_object_get_object(rootObj, name);
				if (itObj)
					(*it)->Load(itObj);
				else
					ret = false;
			}
			delete[] buf;
		}		
	}
	else
	{
		Debug.LogWarning("Config file not found. Default configuration loaded");
	}

	if (ret)
	{
		Debug.Log("Configuration succesfully loaded");
		App->camera->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	}
		

	return ret;
}

bool Application::SaveNow()const
{
	bool ret = true;

	JSON_Value* root = json_value_init_object();
	JSON_Object* rootObj = json_value_get_object(root);

	//Save App config
	JSON_Value* itValue = json_value_init_object();
	JSON_Object* itObj = json_value_get_object(itValue);
	json_object_set_value(rootObj, "Application", itValue);
	json_object_set_number(itObj, "maxFPS", maxFPS);
	json_object_set_string(itObj, "engineName", engineName.data());
	json_object_set_string(itObj, "organization", organization.data());

	std::list<Module*>::const_iterator it;
	for (it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		char* name = (char*)(*it)->getName();

		JSON_Value* itValue = json_value_init_object();
		JSON_Object* itObj = json_value_get_object(itValue);
		json_object_set_value(rootObj, name, itValue);

		ret = (*it)->Save(itObj);
	}

	if (ret)
	{
		int size = json_serialization_size_pretty(root);
		char* jsonFile = new char[size];
		json_serialize_to_buffer_pretty(root, jsonFile, size);
		ret = App->fs->OpenWrite(SETTINGS_FOLDER + std::string("settings.json"), jsonFile);
		delete[] jsonFile;

		if (ret)
			Debug.Log("Configuration succesfully saved");
		else
			Debug.LogError("Failed saving \"%s\"", (std::string(SETTINGS_FOLDER) + std::string("settings.json")).data());
	}
	else
	{
		Debug.LogError("Error: configuration couldn't be saved");
	}

	return ret;
}