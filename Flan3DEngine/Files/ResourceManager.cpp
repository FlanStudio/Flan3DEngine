#include "ResourceManager.h"
#include "Application.h"

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Start()
{
	//OPTIONAL TODO: Omit that step and make it an export button, then clear the library and export all the saved files

	//Scan assets and make a resource copy in Library. They both have to be linked and stored in the map
	
	std::vector<std::string> fullPaths;
	App->fs->getFilesPath(fullPaths);

	for (int i = 0; i < fullPaths.size(); ++i)
	{
		//Detect the extension and call the right exporter
		std::string extension = App->fs->getExt(fullPaths[i]);

		if (extension.empty())
			continue;

		if (App->textures->isSupported(extension))
		{
			//Its a texture. Call Texture Exporter and save a .dds file in library


		}
		else if (extension == ".fbx" || extension == ".FBX")
		{
			//A lot of stuff here. Extract textures, meshes and gameObjects hierarchy, save the possible stuff in Library 
			//and link the fbx with their separate files with a .meta file. After that manage the drag and drop into our hierarchy.
		}

		//TODO: Materials, audio, animations, etc?
	}

	return true;
}

bool ResourceManager::CleanUp()
{

	return true;
}

update_status ResourceManager::PreUpdate(float dt)
{
	//Receive Drop events, create a resources copy in assets and library, then link both. Check for deletion or modifying. FBX = scene + few files, manage the fbx deletion and linking
	std::string dropped = App->input->getFileDropped();
	if (!dropped.empty())
	{
		//Manage imports here, check if the file has already been imported	
	}
		

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
