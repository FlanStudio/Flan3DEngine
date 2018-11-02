#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Application.h"
#include "pcg-c-basic-0.9/pcg_basic.h"


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
	Component(ComponentType type, GameObject* parent, bool active = true) : type(type), gameObject(parent), active(active) { genUUID(); }
	Component(){}
	virtual ~Component() {}
	virtual bool Update(float dt) { return true; }
	void genUUID()
	{
		if (UUID == 0)
			UUID = FLAN::randomUINT32_Range();
	}

public:
	virtual void OnInspector() {}
	virtual void debugDraw() {}
	virtual void Enable() {}
	virtual void Disable() {}
	virtual bool isActive() { return true; }

public:
	ComponentType type = ComponentType::NO_TYPE;
	GameObject* gameObject = nullptr;
	uint UUID = 0u;

protected:
	bool active = true;
};

#endif