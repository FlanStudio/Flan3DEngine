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
	
	/*glBegin(GL_TRIANGLES);
	glVertex2f(-0.5, -0.5);
	glVertex2f(0.5, -0.5);
	glVertex2f(0.0f, 0.5f);
	glEnd();*/
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(-0.25, -0.25);
	glVertex2f(0.25, -0.25);
	glVertex2f(0.25, 0.25);
	glVertex2f(-0.25, 0.25);
	glEnd();


	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	Debug.Log("Hit!");
}

