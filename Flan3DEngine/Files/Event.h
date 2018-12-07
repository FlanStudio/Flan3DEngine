#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

class GameObject;
class Component;
class Resource;

struct FileSystemEvent
{
	uint32_t type;
	const char* file;

	//ONLY USED WHEN FILE_MOVED
	const char* oldLocation;
};

struct GameObjectEvent
{
	uint32_t type;
	GameObject* gameObject;
};

struct ComponentEvent
{
	uint32_t type;
	Component* component;
};

struct ResourceEvent
{
	uint32_t type;
	Resource* resource;
};

enum EventType
{
	INVALID_TYPE = 0,

	//FileSystem Events
	FILE_CREATED = 0x100,
	FILE_DELETED,
	FILE_MOVED,
	FILE_MODIFIED,

	//Time Events
	PLAY = 0x200,
	STOP,
	PAUSE,
	RESUME,
	STEP,

	//GameObject Events
	GO_DESTROYED = 0x300,

	//Component Events
	COMPONENT_DESTROYED = 0x400,

	//Resources Events
	RESOURCE_DESTROYED = 0x500,
	RESOURCE_MODIFIED
};

union Event
{
	uint32_t type;
	FileSystemEvent fileEvent;
	GameObjectEvent goEvent;
	ComponentEvent compEvent;
	ResourceEvent resEvent;
};


#endif