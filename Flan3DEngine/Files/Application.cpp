#include "Application.h"

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio(true);
	scene_intro = new ModuleSceneIntro();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	physics = new ModulePhysics3D();
	editor = new ModuleEditor();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(editor);
	// Scenes
	AddModule(scene_intro);
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
	LOG("Application Start --------------");
	for (it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	ms_timer.Start();

	
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	float timeFrame = 1.0f / maxFPS;
	
	if (dt < timeFrame)
	{
		
		SDL_Delay((timeFrame - dt) * 1000.0f);
	}

	FPS[FPS_index++] = ImGui::GetIO().Framerate;
	if (FPS_index >= FPS.size())
		FPS_index = 0;

	ms[ms_index++] = (timeFrame - dt) * 1000;
	if (ms_index >= ms.size())
		ms_index = 0;

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator it;
	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate(dt);
	}

	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->Update(dt);
	}

	for (it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate(dt);
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
		ret = (*it)->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}