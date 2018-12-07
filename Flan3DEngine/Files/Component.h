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
	CAMERA,
	SCRIPT,
	MAX_TYPE
};

char* ComponentType_toString(ComponentType type);

class Component
{
public:
	Component(ComponentType type, GameObject* gameObject, bool active = true) : type(type), gameObject(gameObject), active(active) { genUUID(); }
	Component() { genUUID(); }
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
	virtual bool isActive() { return active; }
	virtual void Serialize(char*& cursor) const {}

public:
	ComponentType type = ComponentType::NO_TYPE;
	GameObject* gameObject = nullptr;
	UID UUID = 0u;

protected:
	bool active = true;
};

#endif