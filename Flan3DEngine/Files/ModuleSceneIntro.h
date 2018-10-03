#pragma once
#include "Module.h"
#include "Globals.h"

#include "Glew/include/glew.h"

#include "Cube1.h"
#include "Cube2.h"

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	/*Cube s;*/
	PhysBody3D* sensor;
	Cube1 cube;
	Cube2 cube2;
};
