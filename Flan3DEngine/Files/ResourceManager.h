#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "Module.h"
#include <map>

class Resource
{
public:
	enum class ResourceType
	{
		UNKNOWN = -1,
		TEXTURE,
		MESH, 
		AUDIO, 
		SCENE, 
		BONE, 
		ANIMATION, 
	};
	
	Resource(UID uuid, ResourceType type) : uuid(uuid), type(type) {}
	virtual ~Resource() {}

public:
	inline ResourceType getType() const { return type; }
	inline UID getUUID() const { return uuid; }
	inline const char* getFile() const { return file.data(); }
	inline void setFile(char* newLocation) { file = std::string(newLocation); }
	inline const char* getExportedFile() const { return exportedFile.data(); }
	inline bool isLoadedToMemory() const { return amountLoaded > 0; }
	
	uint amountReferences() const;

	bool LoadToMemory();

protected:
	UID uuid = 0u;
	std::string file;
	std::string exportedFile;

	ResourceType type = ResourceType::UNKNOWN;
	uint amountLoaded = 0u;
};

class ResourceManager : public Module
{
public:
	ResourceManager(bool start_enabled = true) : Module("ResourceManager", start_enabled) {}
	~ResourceManager();

	bool Start();
	bool CleanUp();
	update_status PreUpdate(float dt);
	void ReceiveEvent(Event event);

public:
	Resource* Get(UID uuid) const;
	Resource* FindByFile(char* file);

private:
	std::map<UID, Resource*> resources;
};
#endif