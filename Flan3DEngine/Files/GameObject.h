#pragma once

#include <vector>
#include <string>

#include "Component.h"


class GameObject
{
public:
	Component* CreateComponent(ComponentType type);

private:
	bool active = true;
	std::string name;
	std::vector<Component*> components;

	bool hasTransform = false;
	bool hasMaterial = false;
};