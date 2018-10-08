#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "PhysBody3D.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

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

	App->camera->Move(float3(0.0f, 3.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//App->camera->Move(float3(6.55f, -322.21f, 219.16f));
	//App->camera->LookAt(float3(3.33f, -45.71f, 144.40f));


	//sensor = App->physics->AddBody(s, 0.0f);
	/*sensor->SetAsSensor(true);
	sensor->collision_listeners.push_back(this);*/

	cube.Init();
	cube.setScale(.1, .25, .1);
	cube.setColor( .5,0,0,1 );
	cube.Rotate(40, -1, -1, -1);
	cube.setPos(0, .20, 0);

	cube2.Init();
	cube2.setScale(.1, .25, .1);
	cube2.setColor( .5,0,0,1 );
	cube2.Rotate(40, -1, -1, -1);
	//cube2.setPos(-1, 0, 0);

	//----------------------INITIAL GRID------------------------
	grid.Init();
	grid.setColor(255, 255, 255, 0);
	euler.Init();

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
	//cube2.Render();

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();

	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	Debug.Log("Hit!");
}

