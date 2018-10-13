#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#include "Brofiler\Brofiler.h"
#pragma comment( lib, "Brofiler/ProfilerCore32.lib")

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#define CHECKERS 8 * 8

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

	App->camera->Move(float3(0.0f, 5.0f, 5.0f));
	App->camera->LookAt(float3(0, 0, 0));


	//-------------SPAWN HOUSE WITH THE TEXTURE-----------
	App->meshes->LoadFBX("Assets/meshes/BakerHouse.fbx");
	App->textures->LoadTexture("Assets/textures/Baker_house.dds");

	//---------------------------------------------------


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


	GLubyte checkImage[CHECKERS][CHECKERS][4];
	for (int i = 0; i < CHECKERS; i++) {
		for (int j = 0; j < CHECKERS; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

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
	BROFILER_CATEGORY("ModuleSceneIntro", Profiler::Color::DarkViolet)

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();


	return update_status::UPDATE_CONTINUE;
}

