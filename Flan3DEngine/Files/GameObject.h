#pragma once

#include <vector>
#include <string>

#include "Component.h"
#include "ComponentTransform.h"

class GameObject
{
public:

	GameObject(GameObject* parent) : parent(parent) {}
	~GameObject();

public:
	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::string name = "default";
	std::vector<GameObject*> childs;

public:
	Component* CreateComponent(ComponentType type);
	void AddComponent(Component* component);
	void AddChild(GameObject* child);
	void InsertChild(GameObject* child, int pos);
	void ClearChilds();
	void ClearChild(GameObject* child);
	void ClearComponents();
	void ClearComponent(Component* component);
	bool HasChilds() const;
	GameObject* getSelectedGO();
	void OnInspector();
	int getChildPos(GameObject* child) const;
private:
	
public:
	bool selected = false;
	bool treeOpened = false;
private:
	bool active = true;

	std::vector<Component*> components;

	bool hasTransform = false;
	bool hasMaterial = false;
};
int OnInputCallback(ImGuiInputTextCallbackData* callback);