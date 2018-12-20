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
class ComponentMaterial;
class ComponentScript;

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
	GameObject* CreateGameObject(GameObject* parent, bool createMonoObject = true);
	void DestroyGameObject(GameObject* gameObject);
	void AddGameObject(GameObject* gameObject);
	void ClearGameObjects();
	void guiHierarchy();
	void guiInspector();
	void AddComponentGUI();
	void selectGO(GameObject* toSelect);
	void debugDraw();
	GameObject* getRootNode()const { return gameObjects[0]; }
	GameObject* getSelectedGO() const;

	void Serialize() const;
	void DeSerialize(std::string path, std::string extension);

	//Generic method for serializing Scenes/Prefabs. Use this one as much as posible instead of others, they will be deleted in a future
	void SerializeToBuffer(GameObject* gameObject, char*& buffer, uint& size, bool includeRoot = false) const;

	//Generic method for deserializing Scenes/Prefabs. Use this one as much as posible instead of others, they will be deleted in a future
	//  //* root is filled and allocated as a tree containing the hierarchy. 
	//  //* this method does not include the loaded gameObject in the Engine's hierarchy
	void DeSerializeFromBuffer(GameObject*& root, char*& buffer);

	void SerializeToBuffer(char*& buffer, uint& size) const;
	void DeSerializeFromBuffer(char*& buffer);

	void TransformGUI();

	AABB getSceneAABB() const;
	inline std::string getCurrentScene() const { return currentSceneName; }
	inline void setCurrentScene(const std::string& newName)	{currentSceneName = newName;}

	void OnRenameGUI();

public:
	Quadtree quadtree;
	void UpdateQuadtree();

private:
	void PrintHierarchy(GameObject* go);
	void DragDrop(GameObject* go);
	void decomposeScene(std::vector<GameObject*>& gameObject_s = std::vector<GameObject*>(), std::vector<ComponentTransform*>&transforms = std::vector<ComponentTransform*>(), 
		std::vector<ComponentMesh*>&meshes = std::vector<ComponentMesh*>(), std::vector<ComponentCamera*>&cameras = std::vector<ComponentCamera*>(), 
		std::vector<ComponentMaterial*>&materials = std::vector<ComponentMaterial*>(), std::vector<ComponentScript*>& scripts = std::vector<ComponentScript*>()) const;

	//Internal methods to reorder GameObjects in the hierarchy
	void _ReorderGameObject_Pre(GameObject* go);
	void _ReorderGameObject_Post(GameObject* go);

	void InitQuadtree();	

	void DrawGuizmos();

	void ReceiveEvent(Event event);

private:

	GameObject* selectedGO = nullptr;

	ImGuizmo::OPERATION currentGuizmoOperation = ImGuizmo::OPERATION::BOUNDS;
	ImGuizmo::MODE guizmoMode = ImGuizmo::MODE::WORLD;

	float quadTreeUpdateRate = 2.0f;

	GridPrim grid;

	std::string currentSceneName = "defaultScene";
	std::vector<GameObject*> gameObjects;

	char* tempSceneBuffer = nullptr;
	uint tempSceneBufferSize = 0;

	uint TransformAtlasID = 0;

	bool deserialize = false;
};
#endif
