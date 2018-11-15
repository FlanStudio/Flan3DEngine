#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

class GameObject;
class Resource;

struct FileSystemEvent
{
	uint32_t type;
	const char* file;

	//ONLY USED WHEN FILE_MOVED
	const char* oldLocation;
};

struct TimeEvent
{
	uint32_t type;
};

struct GameObjectEvent
{
	uint32_t type;
	GameObject* gameObject;
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

	//Resources Events
	RESOURCE_DESTROYED = 0x400
};

union Event
{
	uint32_t type;
	FileSystemEvent fileEvent;
	TimeEvent timeEvent;
	GameObjectEvent goEvent;
	ResourceEvent resEvent;
};


#endif