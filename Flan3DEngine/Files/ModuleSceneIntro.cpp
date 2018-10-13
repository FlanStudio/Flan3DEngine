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

	/*App->camera->Move(float3(6.55f, -322.21f, 219.16f));
	App->camera->LookAt(float3(3.33f, -45.71f, 144.40f));*/


	//sensor = App->physics->AddBody(s, 0.0f);
	/*sensor->SetAsSensor(true);
	sensor->collision_listeners.push_back(this);*/

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


	//cube.Render();
	//cube2.Render();


	//TODO: FIX VERTEX TO SEE THE TEXTURE RIGHT
	/*{
		glBindTexture(GL_TEXTURE_2D, App->textures->textures.empty() ? 0 : App->textures->textures[0]->id);

		glBegin(GL_TRIANGLES);

		glTexCoord2f(0, 0);	glVertex3f(0, 0, 0);
		glTexCoord2f(1, 0);	glVertex3f(1 , 0, 0);
		glTexCoord2f(0, 1);	glVertex3f(0, 1, 0);
		glTexCoord2f(1, 0);	glVertex3f(1 , 0, 0);
		glTexCoord2f(1, 1);	glVertex3f(1 , 1 , 0);
		glTexCoord2f(0, 1);	glVertex3f(0, 1 , 0); //FRONT

		glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
		glTexCoord2f(1, 1); glVertex3f(1, 0, -1);
		glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
		glTexCoord2f(1, 1); glVertex3f(1, 0, -1);
		glTexCoord2f(0, 1); glVertex3f(1, 1, -1);
		glTexCoord2f(0, 0); glVertex3f(1, 1, 0); //RIGHT

		glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
		glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
		glTexCoord2f(1, 1); glVertex3f(0, 1, -1);
		glTexCoord2f(0, 0); glVertex3f(1, 1, 0);
		glTexCoord2f(1, 0); glVertex3f(1, 1, -1);
		glTexCoord2f(1, 1); glVertex3f(0, 1, -1); //TOP

		glTexCoord2f(0, 0); glVertex3f(1, 0, -1);
		glTexCoord2f(1, 0); glVertex3f(0, 0, -1);
		glTexCoord2f(0, 1); glVertex3f(1, 1, -1);
		glTexCoord2f(1, 0); glVertex3f(0, 0, -1);
		glTexCoord2f(1, 1); glVertex3f(0, 1, -1);
		glTexCoord2f(0, 1); glVertex3f(1, 1, -1); //BACK

		glTexCoord2f(1, 0); glVertex3f(0, 0, -1);
		glTexCoord2f(1, 1); glVertex3f(0, 0, 0);
		glTexCoord2f(0, 0); glVertex3f(0, 1, -1);
		glTexCoord2f(1, 1); glVertex3f(0, 0, 0);
		glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
		glTexCoord2f(0, 0); glVertex3f(0, 1, -1); //LEFT

		glTexCoord2f(1, 0); glVertex3f(0, 0, -1);
		glTexCoord2f(1, 1); glVertex3f(1, 0, -1);
		glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2f(1, 1); glVertex3f(1, 0, -1);
		glTexCoord2f(0, 1); glVertex3f(1, 0, 0);
		glTexCoord2f(0, 0); glVertex3f(0, 0, 0); //BOTTOM


		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
	}*/


	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();


	return update_status::UPDATE_CONTINUE;
}

