#pragma once

class GameObject;

enum class ComponentType
{
	NO_TYPE = -1,
	MESH
};

class Component
{
private:
	ComponentType type = ComponentType::NO_TYPE;
	bool active = true;
	GameObject* gameObject = nullptr;
};