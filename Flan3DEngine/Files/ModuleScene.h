#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

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
	GameObject* CreateGameObject(GameObject* parent);
	void ClearGameObjects();
	void guiHierarchy();
	void guiInspector();
	void selectGO(GameObject* toSelect);
	void debugDraw()const;
private:
	void PrintHierarchy(GameObject* go);
	GameObject* getSelectedGO() const;
	void DragDrop(GameObject* go);

	//Internal methods to reorder GameObjects in the hierarchy
	void _ReorderGameObject_Pre(GameObject* go);
	void _ReorderGameObject_Post(GameObject* go);
};
#endif
