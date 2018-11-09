#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "Module.h"
#include <map>

class Resource;

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