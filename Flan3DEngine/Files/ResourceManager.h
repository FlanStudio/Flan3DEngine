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
	update_status PreUpdate();
	void ReceiveEvent(Event event);

public:
	Resource* Get(UID uuid) const;
	Resource* FindByFile(char* file);

	void InstanciateFBX(const std::string& path) const;

private:
	void deleteEvent(Event event);
	void createEvent(Event event);
	void moveEvent(Event event);
private:
	std::map<UID, Resource*> resources;
};
#endif