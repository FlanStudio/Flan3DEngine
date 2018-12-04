#include "Component.h"
#include "GameObject.h"

class ComponentScript : public Component
{
public:
	ComponentScript(std::string scriptName, GameObject* gameObject = nullptr) : scriptName(scriptName), Component(ComponentType::SCRIPT, gameObject, true) { }

	void Awake();

	void printHelloWorld();

	void OnInspector();

public:
	std::string scriptName;
	std::string csPath;
};