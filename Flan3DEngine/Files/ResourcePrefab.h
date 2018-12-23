#ifndef __RESOURCEPREFAB_H__
#define __RESOURCEPREFAB_H__

#include "Resource.h"

class GameObject;

class ResourcePrefab : public Resource
{
	friend class ResourceManager;
public:
	ResourcePrefab(GameObject* root);
	virtual ~ResourcePrefab() {}

	bool LoadToMemory() { return true; }
	bool UnLoadFromMemory() { return true; }
	uint getBytes() const { return sizeof(ResourcePrefab); }

	void SerializeToBuffer(char*& buffer, uint& size);
	void DeSerializeFromBuffer(char*& buffer);

	inline GameObject* GetRoot() const { return root; }

private:
	GameObject* root = nullptr;
};

#endif