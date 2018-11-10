#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* gameObject, bool active = true) : Component(ComponentType::MATERIAL, gameObject, active) {}

private:
	void OnInspector();

public:
	ResourceTexture* texture = nullptr;
	ImVec4 colorTint = { 1,1,1,1 };

private:
	bool textureClicked = false;

};

#endif