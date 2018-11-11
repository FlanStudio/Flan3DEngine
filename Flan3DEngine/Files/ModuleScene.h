#ifndef __MODULESCENE_H__
#define __MODULESCENE_H__

#include "Module.h"
#include "Globals.h"

#include "Primitives/EulerAxis.h"
#include "Primitives/GridPrim.h"

#include <vector>

#include "Quadtree.h"

#include "imgui/ImGuizmo/ImGuizmo.h"

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
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

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

	void TransformGUI();

public:
	Quadtree quadtree;

private:
	void PrintHierarchy(GameObject* go);
	GameObject* getSelectedGO() const;
	void DragDrop(GameObject* go);
	void decomposeScene(std::vector<GameObject*>& gameObject_s = std::vector<GameObject*>(), std::vector<ComponentTransform*>&transforms = std::vector<ComponentTransform*>(), std::vector<ComponentMesh*>&meshes = std::vector<ComponentMesh*>(), std::vector<ComponentCamera*>&cameras = std::vector<ComponentCamera*>());
	void parentAABBs();

	//Internal methods to reorder GameObjects in the hierarchy
	void _ReorderGameObject_Pre(GameObject* go);
	void _ReorderGameObject_Post(GameObject* go);

	void InitQuadtree();
	void UpdateQuadtree();

	void DrawGuizmos();

private:
	ImGuizmo::OPERATION currentGuizmoOperation = ImGuizmo::OPERATION::BOUNDS;

	float quadTreeUpdateRate = 2.0f;

	EulerAxis euler;
	GridPrim grid;

	std::string currentSceneName = "defaultScene";
	std::vector<GameObject*> gameObjects;
};
#endif
