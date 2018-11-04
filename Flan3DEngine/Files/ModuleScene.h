#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"

#include "Primitives/EulerAxis.h"
#include "Primitives/GridPrim.h"

#include <vector>

#include "Quadtree.h"

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

	Quadtree quadtree;
	void InitQuadtree();
	void UpdateQuadtree();
	float quadTreeUpdateRate = 2.0f;

public:
	GameObject* CreateGameObject(GameObject* parent);
	void ClearGameObjects();
	void guiHierarchy();
	void guiInspector();
	void selectGO(GameObject* toSelect);
	void debugDraw();
	GameObject* getRootNode()const { return gameObjects[0]; }

	void Serialize();
	void DeSerialize(std::string path, std::string extension);

private:
	void PrintHierarchy(GameObject* go);
	GameObject* getSelectedGO() const;
	void DragDrop(GameObject* go);
	void decomposeScene(std::vector<GameObject*>& gameObject_s = std::vector<GameObject*>(), std::vector<ComponentTransform*>&transforms = std::vector<ComponentTransform*>(), std::vector<ComponentMesh*>&meshes = std::vector<ComponentMesh*>(), std::vector<ComponentCamera*>&cameras = std::vector<ComponentCamera*>());
	void parentAABBs();

	//Internal methods to reorder GameObjects in the hierarchy
	void _ReorderGameObject_Pre(GameObject* go);
	void _ReorderGameObject_Post(GameObject* go);
};
#endif
