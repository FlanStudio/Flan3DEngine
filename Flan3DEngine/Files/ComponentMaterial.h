#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* gameObject, bool active = true) : Component(ComponentType::MATERIAL, gameObject, active) {}
	virtual ~ComponentMaterial();

public:
	inline static uint bytesToSerialize() { return sizeof(UID) * 3 + sizeof(ImVec4); } //Your uid, your gameObject uid, your Texture uid, your tint color
	void Serialize(char*& cursor) const;
	void DeSerialize(char*& cursor, uint32_t& goUUID);

private:
	void OnInspector();

public:
	ResourceTexture* texture = nullptr;
	ImVec4 colorTint = { 1,1,1,1 };

private:
	bool textureClicked = false;

};

#endif