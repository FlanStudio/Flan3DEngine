#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Start()
{
	//Scan assets and make a resource copy in Library. They both have to be linked and stored in the map

	return true;
}

bool ResourceManager::CleanUp()
{

	return true;
}

update_status ResourceManager::PreUpdate(float dt)
{
	//Receive Drop events, create a resources copy in library and link both. Check for deletion or modifying. FBX = scene + few files

	return update_status::UPDATE_CONTINUE;
}

void ResourceManager::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::FILE_DELETED:
		{
			Resource* toDelete = FindByFile((char*)event.fileEvent.file);			
			if (toDelete)
			{
				//TODO: ALERT ALL THE REFERENCES TO STOP USING THIS DELETED RESOURCE
				//TODO: DELETE THE BINARY FORMAT FILE
			}
			break;
		}
		case EventType::FILE_CREATED:
		{
			//TODO: CHECK THE FORMAT AND EXPORT IN OUR BINARY ONE. STORE IT IN THE RESOURCES MAP.
			break;
		}
		case EventType::FILE_MOVED:
		{
			//TODO: DUPLICATE THE BEHAVIOR IN LIBRARY?
			Resource* toUpdate = FindByFile((char*)event.fileEvent.oldLocation);
			if(toUpdate)
				toUpdate->setFile((char*)event.fileEvent.file);
			break;
		}
		case EventType::FILE_MODIFIED:
		{
			break;
		}
	}
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

Resource* ResourceManager::FindByFile(char* file)
{
	std::map<UID, Resource*>::iterator it;
	for (it = resources.begin(); it != resources.end(); ++it)
	{
		Resource* resource = it->second;
		if (strcmp(resource->getFile(), file) == 0)
			return resource;
	}
	return nullptr;
}

uint Resource::amountReferences() const
{
	return 0;
}

bool Resource::LoadToMemory()
{
	return true;
}
