#ifndef __RESOURCESCRIPT_H__
#define __RESOURCESCRIPT_H__

#include "Resource.h"

class ResourceScript : public Resource
{
public:

	enum class ScriptState
	{
		NO_STATE = -1,
		NO_COMPILED,
		COMPILED_WITH_ERRORS,
		COMPILED_FINE
		
	} state = ScriptState::NO_COMPILED;

	ResourceScript() : Resource(ResourceType::SCRIPT) {}
	virtual ~ResourceScript();

	void SerializeToMeta(char*& cursor) const;
	void DeSerializeFromMeta(char*& cursor);
	uint bytesToSerializeMeta() const;

public:
	bool compilednoerrors = true;

private:
	uint getBytes() const;
};

#endif