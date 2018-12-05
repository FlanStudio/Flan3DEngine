#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <string>
#include "pcg-c-basic-0.9/pcg_basic.h"

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
		SCRIPT
	};

	Resource(ResourceType type) : type(type) 
	{
		uuid = FLAN::randomUINT32_Range();
	}
	virtual ~Resource() {}

public:
	inline ResourceType getType() const { return type; }
	inline UID getUUID() const { return uuid; }
	inline const char* getFile() const { return file.data(); }
	inline void setFile(char* newLocation) { file = std::string(newLocation); }
	inline void setExportedFile(char* newLocation) { exportedFile = std::string(newLocation); }
	inline const char* getExportedFile() const { return exportedFile.data(); }
	inline bool isLoadedToMemory() const { return timesReferenced > 0; }
	inline void setUID(UID uid) { uuid = uid; }

	uint amountReferences() const { return timesReferenced; }
	
	virtual uint getBytes() const = 0;
	
	void Referenced();
	void deReferenced();

protected:
	virtual bool LoadToMemory() = 0;
	virtual bool UnLoadFromMemory() = 0;

protected:

	UID uuid = 0u;
	std::string file;
	std::string exportedFile;

	ResourceType type = ResourceType::UNKNOWN;
	uint timesReferenced = 0u;
};

#endif