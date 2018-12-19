#include "Component.h"
#include "GameObject.h"

struct _MonoObject;
class ResourceScript;

class ComponentScript : public Component
{
public:
	ComponentScript(std::string scriptName, GameObject* gameObject = nullptr) : scriptName(scriptName), Component(ComponentType::SCRIPT, gameObject, true) { }
	virtual ~ComponentScript();

	void Awake();
	void Start();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void OnEnable();
	void OnDisable();
	void OnStop();
	void onEnable()override;
	void onDisable()override;

	void OnInspector();

	static uint bytesToSerialize() { return sizeof(UID) * 3 + (sizeof(bool)); }
	void Serialize(char*& cursor) const;
	void deSerialize(char*& cursor, uint32_t& goUUID);

public:
	void InstanceClass();

public:
	bool awaked = false;
	std::string scriptName;

	ResourceScript* scriptRes = nullptr;

	uint32_t handleID = 0;
	_MonoObject* classInstance = nullptr;

private:
	
};