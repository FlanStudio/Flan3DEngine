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

	Component* CreateComponent(ComponentType type);
	void AddComponent(Component* component);
	void AddChild(GameObject* child);

public:
	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::string name = "default";
	std::vector<GameObject*> childs;
	void ClearChilds();
	void ClearComponents();
	void ClearComponent(Component* component);
private:
	bool active = true;

	std::vector<Component*> components;

	bool hasTransform = false;
	bool hasMaterial = false;
};