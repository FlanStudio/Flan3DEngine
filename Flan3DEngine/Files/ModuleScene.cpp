#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "Brofiler\Brofiler.h"

#include "GameObject.h"

#define CHECKERS 8 * 8

ModuleScene::ModuleScene(bool start_enabled) : Module("ModuleSceneIntro", start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	Debug.Log("Loading Intro assets");
	bool ret = true;

	App->camera->Move(float3(0.0f, 5.0f, 5.0f));
	App->camera->LookAt(float3(0, 0, 0));


	//-------------SPAWN HOUSE WITH THE TEXTURE-----------
	App->fbxLoader->LoadFBX("Assets/meshes/BakerHouse.fbx");
	App->textures->LoadTexture("Assets/textures/Baker_house.dds");


	//----------------------INITIAL GRID------------------------
	grid.Init();
	grid.setColor(255, 255, 255, 0);
	euler.Init();


	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	Debug.Log("Unloading Intro scene");

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleSceneIntro", Profiler::Color::DarkViolet)

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();


	return update_status::UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject()
{
	GameObject* ret = new GameObject();
	gameObjects.push_back(ret);
	return ret;
}

