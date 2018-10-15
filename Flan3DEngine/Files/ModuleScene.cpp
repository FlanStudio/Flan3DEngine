#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "Brofiler\Brofiler.h"

#include "GameObject.h"
#include "ComponentTransform.h"

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

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* ret = new GameObject(parent);

	if(parent)
		parent->AddChild(ret);
	else
		gameObjects.push_back(ret);

	ret->CreateComponent(ComponentType::TRANSFORM); //All GameObjects have a Transform

	return ret;
}

void ModuleScene::guiMeshesTransform()const
{
	/*for (int i = 0; i < gameObjects.size(); ++i)
	{
		ImGui::Text("Mesh %i: %s", i, gameObjects[i]->name.data());
		ImGui::Text("Position: %.2f,%.2f,%.2f", gameObjects[i]->transform->position.x, gameObjects[i]->transform->position.y, gameObjects[i]->transform->position.z);
		float3 angles = gameObjects[i]->transform->rotation.ToEulerXYZ();
		ImGui::Text("Rotation: %.2f,%.2f,%.2f", RadToDeg(angles.x), RadToDeg(angles.y), RadToDeg(angles.z));
		ImGui::Text("Scale: %.2f,%.2f,%.2f", gameObjects[i]->transform->scale.x, gameObjects[i]->transform->scale.y, gameObjects[i]->transform->scale.z);
		ImGui::NewLine();
	}*/
	PrintHierarchy(gameObjects[0]);
}

void ModuleScene::PrintHierarchy(GameObject* go)const
{
	for (int i = 0; i < go->childs.size(); ++i)
	{
		PrintHierarchy(go->childs[i]);
	}

	if (go == gameObjects[0]) //Dont print the rootNode
		return;

	ImGui::Text("GameObject: %s", go->name.data());
	ImGui::Text("Position: %.2f,%.2f,%.2f", go->transform->position.x, go->transform->position.y, go->transform->position.z);
	float3 angles = go->transform->rotation.ToEulerXYZ();
	ImGui::Text("Rotation: %.2f,%.2f,%.2f", RadToDeg(angles.x), RadToDeg(angles.y), RadToDeg(angles.z));
	ImGui::Text("Scale: %.2f,%.2f,%.2f", go->transform->scale.x, go->transform->scale.y, go->transform->scale.z);
	ImGui::NewLine();
}

void ModuleScene::ClearGameObjects()
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		delete gameObjects[i];
	}
	gameObjects.clear();
}
