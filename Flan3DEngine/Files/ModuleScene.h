#pragma once
#include "Module.h"
#include "Globals.h"

#include "Primitives/EulerAxis.h"
#include "Primitives/GridPrim.h"

#include <vector>

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:
	EulerAxis euler;
	GridPrim grid;

	std::vector<GameObject*> gameObjects;

public:
	GameObject* CreateGameObject();

};
