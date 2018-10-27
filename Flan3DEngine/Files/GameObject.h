#pragma once

#include <vector>
#include <string>

#include "Component.h"
#include "ComponentTransform.h"

#include "MathGeoLib_1.5/Math/float3x3.h"

class GameObject
{
public:

	GameObject(GameObject* parent) : parent(parent) 
	{
		boundingBox.SetNegativeInfinity();
	}

	~GameObject();

public:
	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::string name = "default";
	std::vector<GameObject*> childs;
	AABB boundingBox;

public:
	Component* CreateComponent(ComponentType type);
	void AddComponent(Component* component);
	void AddChild(GameObject* child);
	Component* getComponentByType(ComponentType type) const;
	void InsertChild(GameObject* child, int pos);
	void ClearChilds();
	void ClearChild(GameObject* child);
	void ClearComponents();
	void ClearComponent(Component* component);
	bool HasChilds() const;
	GameObject* getSelectedGO() const;
	void OnInspector();
	int getChildPos(const GameObject* child) const;
	void InsertComponent(Component* component, int pos);
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