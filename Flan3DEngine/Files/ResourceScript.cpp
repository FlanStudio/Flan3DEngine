#include "ResourceScript.h"

ResourceScript::~ResourceScript()
{
	
}

void ResourceScript::SerializeToMeta(char*& cursor) const
{
	uint bytes = sizeof(UID);
	memcpy(cursor, &uuid, bytes);
	cursor += bytes;
}

void ResourceScript::DeSerializeFromMeta(char*& cursor)
{
	uint bytes = sizeof(UID);
	memcpy(&uuid, cursor, bytes);
	cursor += bytes;



}

uint ResourceScript::bytesToSerializeMeta() const
{
	//Store the UID to get the .dll file
	return sizeof(UID);
}

uint ResourceScript::getBytes() const
{
	return sizeof(ResourceScript);
}