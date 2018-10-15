#pragma once

#include <vector>
#include "Component.h"

class Transform;
class GameObject
{
public:
	Component* CreateComponent(ComponentType type);

private:
	Transform* transform;
	std::vector<Component*> components;
};