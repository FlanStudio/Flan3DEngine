#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Start()
{
	//Scan assets and make a resource copy in Library. They both have to be linked and stored in the map

	return false;
}

bool ResourceManager::CleanUp()
{

	return false;
}

update_status ResourceManager::PreUpdate(float dt)
{
	//Receive Drop events, create a resources copy in library and link both. Check for deletion or modifying. FBX = scene + few files

	return update_status::UPDATE_CONTINUE;
}

Resource* ResourceManager::Get(UID uuid) const
{
	Resource* ret = nullptr;

	std::map<UID, Resource*>::const_iterator it = resources.find(uuid);
	if (it != resources.end())
	{
		ret = it->second;
	}

	return ret;
}

uint Resource::amountReferences() const
{
	return uint();
}

bool Resource::LoadToMemory()
{
	return false;
}
