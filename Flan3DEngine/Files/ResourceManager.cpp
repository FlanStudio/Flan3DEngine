#include "ResourceManager.h"
#include "Application.h"

#include "Resource.h"
#include "ResourceTexture.h"

#include "Brofiler/Brofiler.h"

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Start()
{
	//OPTIONAL TODO: Omit that step and make it an export button, then clear the library and export all the saved files. Exporting big textures takes a while.

	//Scan assets and make a resource copy in Library. They both have to be linked and stored in the map
	
	BROFILER_CATEGORY("ResourceManager_Start", Profiler::Color::Black)

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
			ResourceTexture* textureR = App->textures->ExportResource(fullPaths[i]);
			resources.insert(std::pair<UID, Resource*>(textureR->getUUID(), textureR));

			//Save a .meta to link both files through the UID
			char* buffer;
			uint size = 0;
			size += sizeof(UID);

			buffer = new char[size];
			UID uid = textureR->getUUID();
			memcpy(buffer, &uid, size);

			App->fs->OpenWriteBuffer(fullPaths[i] + std::string(".meta"), buffer, size);
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

update_status ResourceManager::PreUpdate()
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
	char* buffer = nullptr;
	int size;

	App->fs->OpenRead(file + std::string(".meta"), &buffer, size);

	if (!buffer)
		return nullptr;

	UID uuid;
	memcpy(&uuid, buffer, sizeof(UID));

	delete buffer;

	return resources.at(uuid);
}

uint Resource::amountReferences() const
{
	return 0;
}

bool Resource::LoadToMemory()
{
	return true;
}
