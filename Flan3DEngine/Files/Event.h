#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

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
	STEP
};

union Event
{
	uint32_t type;
	FileSystemEvent fileEvent;
	TimeEvent timeEvent;
};



#endif