#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module("ModuleSceneIntro", start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	Debug.Log("Loading Intro assets");
	bool ret = true;

	/*App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));*/

	//s.size = vec3(5, 3, 1);
	//s.SetPos(0, 2.5f, 20);

	//sensor = App->physics->AddBody(s, 0.0f);
	/*sensor->SetAsSensor(true);
	sensor->collision_listeners.push_back(this);*/

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	Debug.Log("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Plane p(0, 1, 0, 0);
	/*p.axis = true;
	p.Render();*/

	/*sensor->GetTransform(&s.transform);
	s.Render();*/

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	Debug.Log("Hit!");
}

