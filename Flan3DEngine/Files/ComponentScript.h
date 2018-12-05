#include "Component.h"
#include "GameObject.h"

struct _MonoMethod;
struct _MonoObject;
struct _MonoImage;

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

	//Return false if the cs file has errors
	bool CompileCSFile();

public:
	bool initialized = false;
	std::string scriptName;
	std::string csPath;

private:
	_MonoMethod* awakeMethod = nullptr;
	_MonoMethod* startMethod = nullptr;
	_MonoMethod* preUpdateMethod = nullptr;
	_MonoMethod* updateMethod = nullptr;
	_MonoMethod* postUpdateMethod = nullptr;

	_MonoAssembly* assembly = nullptr;
	_MonoImage* image = nullptr;

	_MonoObject* classInstance = nullptr;
};