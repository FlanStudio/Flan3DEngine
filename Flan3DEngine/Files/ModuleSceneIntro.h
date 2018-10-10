#pragma once
#include "Module.h"
#include "Globals.h"

#include "Glew/include/glew.h"

#include "Primitives/Cube1.h"
#include "Primitives/Cube2.h"

#include "Primitives/EulerAxis.h"
#include "Primitives/GridPrim.h"



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

public:

	Cube1 cube;
	Cube2 cube2;

	EulerAxis euler;
	GridPrim grid;
};
