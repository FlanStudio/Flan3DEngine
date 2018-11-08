#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

struct FileSystemEvent
{
	uint32_t type;
	const char* fileChanged;
};

enum EventType
{
	INVALID_TYPE = 0,

	//FileSystem Events
	FILE_CREATED = 0x100,
	FILE_DELETED,
	FILE_MOVED
};

union Event
{
	uint32_t type;
	FileSystemEvent fileEvent;
};



#endif