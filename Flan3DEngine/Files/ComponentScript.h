#include "Component.h"
#include "GameObject.h"

struct _MonoObject;
class ResourceScript;

class ComponentScript : public Component
{
public:
	ComponentScript(std::string scriptName, GameObject* gameObject = nullptr) : scriptName(scriptName), Component(ComponentType::SCRIPT, gameObject, true) { }

	void Awake();
	void Start();
	void PreUpdate();
	void Update();
	void PostUpdate();

	void OnInspector();

public:
	void InstanceClass();

public:
	bool awaked = false;
	std::string scriptName;

	ResourceScript* scriptRes = nullptr;
	
private:
	_MonoObject* classInstance = nullptr;
};