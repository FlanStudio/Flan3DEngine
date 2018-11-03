#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"

#include "Primitives/EulerAxis.h"
#include "Primitives/GridPrim.h"

#include <vector>

class GameObject;
class ComponentTransform;
class ComponentMesh;
class ComponentCamera;

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

	std::string currentSceneName = "defaultScene";
	std::vector<GameObject*> gameObjects;
public:
	GameObject* CreateGameObject(GameObject* parent);
	void ClearGameObjects();
	void guiHierarchy();
	void guiInspector();
	void selectGO(GameObject* toSelect);
	void debugDraw()const;

	void Serialize();
	void DeSerialize(std::string path, std::string extension);

private:
	void PrintHierarchy(GameObject* go);
	GameObject* getSelectedGO() const;
	void DragDrop(GameObject* go);
	void decomposeScene(std::vector<GameObject*>&, std::vector<ComponentTransform*>&, std::vector<ComponentMesh*>&, std::vector<ComponentCamera*>&);
	void parentAABBs();


	//Internal methods to reorder GameObjects in the hierarchy
	void _ReorderGameObject_Pre(GameObject* go);
	void _ReorderGameObject_Post(GameObject* go);
};
#endif
