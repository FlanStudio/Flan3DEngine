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

	App->camera->Move(float3(0.0f, 5.0f, 5.0f));
	App->camera->LookAt(float3(0, 0, 0));

	/*App->camera->Move(float3(6.55f, -322.21f, 219.16f));
	App->camera->LookAt(float3(3.33f, -45.71f, 144.40f));*/


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


	GLubyte checkImage[10][10][4];
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	uint ImageName;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ImageName);
	glBindTexture(GL_TEXTURE_2D, ImageName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

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


	//TODO: FIX VERTEX TO SEE THE TEXTURE RIGHT
	{
		glBindTexture(GL_TEXTURE_2D, 1);

		glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 0); //FRONT

		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(1, 1, 0); //RIGHT

		glVertex3f(0, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, 1);
		glVertex3f(0, 1, 1); //TOP

		glVertex3f(1, 0, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 1); //BACK

		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 0); //BOTTOM

		glVertex3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, 1); //LEFT

		glTexCoord2f(0, 0);//Front v
		glTexCoord2f(1, 0);
		glTexCoord2f(0, 1);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 1);

		glTexCoord2f(0, 1); //Right x
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);

		glTexCoord2f(0, 0);//top v
		glTexCoord2f(1, 0);
		glTexCoord2f(0, 1);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 1);

		glTexCoord2f(0, 1); //back x
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);

		glTexCoord2f(0, 1); //bottom x
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);

		glTexCoord2f(0, 0);//left v
		glTexCoord2f(1, 0);
		glTexCoord2f(0, 1);
		glTexCoord2f(1, 0);
		glTexCoord2f(1, 1);
		glTexCoord2f(0, 1);



		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();


	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	Debug.Log("Hit!");
}
