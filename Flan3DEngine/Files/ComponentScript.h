#include "Component.h"
#include "GameObject.h"

class ComponentScript : public Component
{
public:
	ComponentScript(std::string csPath, GameObject* gameObject = nullptr) : csPath(csPath), Component(ComponentType::SCRIPT, gameObject, true) { }

public:
	std::string csPath;
};