#pragma once

#include "Application.h"

class GameObject;

enum class ComponentType
{
	NO_TYPE = -1,
	TRANSFORM,
	MESH
};

class Component
{
public:
	Component(ComponentType type, GameObject* parent, bool active = true) : type(type), gameObject(parent), active(active){}

public:
	ComponentType type = ComponentType::NO_TYPE;

public:
	bool active = true;
	GameObject* gameObject = nullptr;
};