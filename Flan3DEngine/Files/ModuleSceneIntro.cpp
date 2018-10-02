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

	/*App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));*/

	//sensor = App->physics->AddBody(s, 0.0f);
	/*sensor->SetAsSensor(true);
	sensor->collision_listeners.push_back(this);*/

	cube.Init();
	cube.setScale(0.25, 0.25, 0.25);
	cube.Rotate(40, -1, -1, -1);

	cube2.Init();
	cube2.setScale(.1, .1, .1);
	cube2.setColor({ .5,0,0,1 });
	cube2.Rotate(40, 1, 0, 1);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	Debug.Log("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{

	
	return update_status::UPDATE_CONTINUE;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	
	
	

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{	
	//cube.Render();
	cube2.Render();


	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	Debug.Log("Hit!");
}

