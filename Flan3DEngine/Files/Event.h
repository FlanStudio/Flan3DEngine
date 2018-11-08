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

enum EventType
{
	INVALID_TYPE = 0,

	//FileSystem Events
	FILE_CREATED = 0x100,
	FILE_DELETED,
	FILE_MOVED,
	FILE_MODIFIED //TODO: HOW TO DETECT THAT?
};

union Event
{
	uint32_t type;
	FileSystemEvent fileEvent;
};



#endif