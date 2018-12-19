#ifndef __RESOURCEPREFAB_H__
#define __RESOURCEPREFAB_H__

#include "Resource.h"

class GameObject;

class ResourcePrefab : public Resource
{
public:
	ResourcePrefab() : Resource(ResourceType::PREFAB) {}
	virtual ~ResourcePrefab() {}

	bool LoadToMemory() { return true; }
	bool UnLoadFromMemory() { return true; }

	void SerializeToBuffer(char*& buffer, uint& size);
	void DeSerializeFromBuffer(char*& buffer);

private:
	GameObject* root = nullptr;
};

#endif