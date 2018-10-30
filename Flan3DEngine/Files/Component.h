#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Application.h"

class GameObject;

enum class ComponentType
{
	NO_TYPE = -1,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA
};

class Component
{
public:
	Component(ComponentType type, GameObject* parent, bool active = true) : type(type), gameObject(parent), active(active){}
	Component(){}
	virtual ~Component() {}
	virtual bool Update(float dt) { return true; }
public:
	ComponentType type = ComponentType::NO_TYPE;

public:
	bool active = true;
	GameObject* gameObject = nullptr;

public:
	virtual void OnInspector() {}
	virtual void debugDraw() {}
};

#endif