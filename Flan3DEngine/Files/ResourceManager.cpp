#include "ResourceManager.h"
#include "Application.h"

#include "Resource.h"
#include "ResourceTexture.h"

#include "Brofiler/Brofiler.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"

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
			continue; //Let the fbx to the end so all the other resources can be loaded first
		}
		//TODO: Materials, audio, animations, etc?
	}

	for (int i = 0; i < fullPaths.size(); ++i)
	{
		//Detect the extension and call the right exporter
		std::string extension = App->fs->getExt(fullPaths[i]);

		if (extension.empty())
			continue;

		if (extension == ".fbx" || extension == ".FBX")
		{
			//A lot of stuff here. Extract textures, meshes and gameObjects hierarchy, save the possible stuff in Library 
			//and link the fbx with their separate files with a .meta file. After that manage the drag and drop into our hierarchy.

			//TODO: ANOTHER FBX USING THE SAME RESOURCES?

			std::vector<Resource*> exportedRes = App->fbxexporter->ExportFBX(fullPaths[i]);
			for (int j = 0; j < exportedRes.size(); ++j)
			{
				resources.insert(std::pair<UID, Resource*>(exportedRes[j]->getUUID(), exportedRes[j]));
			}
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
		std::string extension = App->fs->getExt(dropped);
		if (!extension.empty())
		{
			if (extension == ".fbx" || extension == ".FBX")
			{
				uint fileNamePos = dropped.find_last_of("/");
				std::string directory = dropped.substr(0, fileNamePos + 1);
				std::string fileName = dropped.substr(fileNamePos + 1);

				App->fs->UpdateAssetsDir();

				std::vector<std::string> files;
				App->fs->getFilesPath(files);

				for (int i = 0; i < files.size(); ++i)
				{
					std::string assets_FileName = files[i].substr(files[i].find_last_of("/") + 1);
					if (assets_FileName == fileName)
						return UPDATE_CONTINUE;
				}

				App->fs->BeginTempException(directory);
				
				char* metaBuffer;
				uint metaSize;
				std::vector<Resource*> exportedRes = App->fbxexporter->ExportFBX("Exception/" + fileName, metaBuffer, metaSize);
				for (int j = 0; j < exportedRes.size(); ++j)
				{
					resources.insert(std::pair<UID, Resource*>(exportedRes[j]->getUUID(), exportedRes[j]));
				}

				App->fs->CopyExternalFileInto(dropped, "Assets/meshes/");
				
				App->fs->OpenWriteBuffer("Assets/meshes/" + fileName + ".meta", metaBuffer, metaSize);

				App->fs->EndTempException();
			}
			else
			{
				if(App->textures->isSupported(extension))
				{				
					//Check if this texture is already in Assets

					uint fileNamePos = dropped.find_last_of("/");
					std::string directory = dropped.substr(0, fileNamePos + 1);
					std::string fileName = dropped.substr(fileNamePos + 1);

					App->fs->UpdateAssetsDir();

					std::vector<std::string> files;
					App->fs->getFilesPath(files);

					for (int i = 0; i < files.size(); ++i)
					{
						std::string assets_FileName = files[i].substr(files[i].find_last_of("/") + 1);
						if (assets_FileName == fileName)
							return UPDATE_CONTINUE;
					}

					//If not, copy it and export

					App->fs->BeginTempException(directory);
					App->fs->CopyExternalFileInto(dropped, "Assets/textures/");
					App->fs->EndTempException();

					ResourceTexture* texture = App->textures->ExportResource("Assets/textures/" + fileName);
					
					resources.insert(std::pair<UID, Resource*>(texture->getUUID(), texture));

					char* buffer;
					uint size = sizeof(uint);
					buffer = new char[size];
					UID textureUID = texture->getUUID();
					memcpy(buffer, &textureUID, size);

					App->fs->OpenWriteBuffer("Assets/textures/" + fileName + ".meta", buffer, size);

				}
			}
		}
	}
	
	return update_status::UPDATE_CONTINUE;
}

void ResourceManager::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::FILE_DELETED:
		{
			std::string ext = App->fs->getExt(event.fileEvent.file);
			if (ext == ".fbx" || ext == ".FBX")
			{
				//Delete all their meshes and the .meta

				char* metaBuffer;
				int size;
				if (App->fs->OpenRead(std::string(event.fileEvent.file) + ".meta", &metaBuffer, size))
				{
					char* cursor = metaBuffer;
					uint numGameObjects;
					
					uint bytes = sizeof(uint);
					memcpy(&numGameObjects, cursor, bytes);
					cursor += bytes;

					for (int i = 0; i < numGameObjects; ++i)
					{
						cursor += 100; //Skip the name
						bytes = sizeof(UID);
						cursor += bytes * 2; //Skip your and your parent's UID

						bytes = sizeof(float3) * 2 + sizeof(Quat); 
						cursor += bytes; //Skip the transformation

						bytes = sizeof(UID);

						uint meshUID;
						memcpy(&meshUID, cursor, bytes); //Obtain your mesh UID
						cursor += bytes;

						uint textureUID;
						memcpy(&textureUID, cursor, bytes); //Obtain your texture UID
						cursor += bytes;
						
						if (meshUID != 0 && resources.find(meshUID) != resources.end()) //If this gameObject has a non-deleted mesh referenced
						{						
							//Delete the resource
							App->fs->deleteFile(MESHES_LIBRARY_FOLDER + std::to_string(meshUID) + MESHES_EXTENSION);
							Resource* meshResource = resources.at(meshUID);
							resources.erase(meshUID);
							delete meshResource;

							//Alert all the references to stop usign this deleted resource
							Event event;
							event.resEvent.type = EventType::RESOURCE_DESTROYED;
							event.resEvent.resource = meshResource;
						}

						//NOTE: Due to the way assimp manages fbx, their textures original files have to be always on disk, in order to detect file modification and being able of re-export the fbx. 
						//We consider their relationship as a reference, so this .meta only says which texture are their gameObjects relating to. 
						//As this texture is not part of this fbx, we won't delete it and will remain on disk.

						/*if (textureUID != 0 && resources.find(textureUID) != resources.end()) //If this gameObject has a non-deleted texture referenced
						{
							//Delete the resource
							App->fs->deleteFile(TEXTURES_LIBRARY_FOLDER + std::to_string(textureUID) + TEXTURES_EXTENSION);
							Resource* textureResource = resources.at(textureUID);
							resources.erase(textureUID);
							delete textureResource;
							//NOTE: If this fbx was dropped here
						}*/		
					}

					delete metaBuffer;
					App->fs->deleteFile(std::string(event.fileEvent.file) + ".meta");
				}
			}
			else
			{
				Resource* toDelete = FindByFile((char*)event.fileEvent.file);
				if (toDelete)
				{
					//This should probably be a texture for now

					//Alert all the references to stop usign this deleted resource
					Event event;
					event.resEvent.type = EventType::RESOURCE_DESTROYED;
					event.resEvent.resource = toDelete;

					//Delete the resource from library and from memory
					switch (toDelete->getType())
					{
						case Resource::ResourceType::TEXTURE:
						{
							App->fs->deleteFile(std::string(event.fileEvent.file) + ".meta");
							resources.erase(toDelete->getUUID());
							delete toDelete;
							break;
						}
					}
				}
			}

			break;
		}
		case EventType::FILE_CREATED:
		{
			//Check the format and export the file in our binary one. Store the loaded resources into the resources map.

			std::string file = event.fileEvent.file;
			std::string ext = App->fs->getExt(file);

			if (ext == ".fbx" || ext == ".FBX")
			{
				std::vector<Resource*> exportedRes = App->fbxexporter->ExportFBX(file);
				for (int j = 0; j < exportedRes.size(); ++j)
				{
					resources.insert(std::pair<UID, Resource*>(exportedRes[j]->getUUID(), exportedRes[j]));
				}
			}
			else if (App->textures->isSupported(ext))
			{
				ResourceTexture* textureRes = App->textures->ExportResource(file);
				resources.insert(std::pair<UID, Resource*>(textureRes->getUUID(), textureRes));
			}
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

	return	resources.find(uuid) != resources.end() ? resources.at(uuid) : nullptr;
}

void ResourceManager::InstanciateFBX(const std::string& path) const
{
	char* metaBuffer = nullptr;
	int metaBufferSize;
	App->fs->OpenRead(path + ".meta", &metaBuffer, metaBufferSize);

	if (!metaBuffer)
		return;

	char* cursor = metaBuffer;

	uint numGOs;
	memcpy(&numGOs, cursor, sizeof(uint));
	cursor += sizeof(uint);

	struct goInfo
	{
		std::string name;
		UID uid;
		UID parentUID;
		UID textureUID;
		UID meshUID;

		float3 position, scale;
		Quat rotation;
	};

	std::vector<goInfo> gos(numGOs);

	for (int j = 0; j < numGOs; ++j)
	{
		uint bytes = 100;
		gos[j].name.resize(100);
		memcpy((void*)gos[j].name.c_str(), cursor, 100);
		cursor += 100;

		bytes = sizeof(UID);
		memcpy(&gos[j].uid, cursor, bytes);
		cursor += bytes;

		memcpy(&gos[j].parentUID, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float3);
		memcpy(&gos[j].position, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(Quat);
		memcpy(&gos[j].rotation, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float3);
		memcpy(&gos[j].scale, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(UID);
		memcpy(&gos[j].meshUID, cursor, bytes);
		cursor += bytes;

		memcpy(&gos[j].textureUID, cursor, bytes);
		cursor += bytes;
	}
	delete metaBuffer;

	std::vector<GameObject*> gameObjects;

	for (int j = 0; j < gos.size(); ++j)
	{
		GameObject* gameObject = new GameObject(nullptr);
		gameObject->uuid = gos[j].uid;

		ComponentTransform* transform = (ComponentTransform*)gameObject->CreateComponent(ComponentType::TRANSFORM);
		transform->position = gos[j].position;
		transform->rotation = gos[j].rotation;
		transform->scale = gos[j].scale;

		UID meshUID = gos[j].meshUID;
		if (meshUID != 0)
		{
			ComponentMesh* meshComp = (ComponentMesh*)gameObject->CreateComponent(ComponentType::MESH);
			meshComp->mesh = (ResourceMesh*)resources.at(meshUID);
		}

		UID textureUID = gos[j].textureUID;
		if (textureUID != 0)
		{
			ComponentMaterial* matComp = (ComponentMaterial*)gameObject->CreateComponent(ComponentType::MATERIAL);
			matComp->texture = (ResourceTexture*)resources.at(textureUID);
		}

		gameObject->name = gos[j].name;

		gameObjects.push_back(gameObject);
	}

	for (int j = 0; j < gos.size(); ++j)
	{
		for (int k = 0; k < gameObjects.size(); ++k)
		{
			if (gos[j].parentUID == gameObjects[k]->uuid)
			{
				gameObjects[j]->parent = gameObjects[k];
				gameObjects[k]->AddChild(gameObjects[j]);
				break;
			}
		}
	}

	GameObject* root;
	for (int j = 0; j < gameObjects.size(); ++j)
	{
		if (gameObjects[j]->parent == nullptr)
		{
			root = gameObjects[j];
		}
		gameObjects[j]->transformAABB();
		//We need to re-Randomize here the UUID in order to evade duplicates with the pre-Loaded hierarchy
		gameObjects[j]->genUUID();
	}

	App->scene->AddGameObject(root);
}

uint Resource::amountReferences() const
{
	return 0;
}

bool Resource::LoadToMemory()
{
	return true;
}
