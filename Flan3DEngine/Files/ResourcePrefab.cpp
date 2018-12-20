#include "ResourcePrefab.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentMaterial.h"
#include "ComponentScript.h"

ResourcePrefab::ResourcePrefab(GameObject* goTemplate) : Resource(ResourceType::PREFAB) 
{
	if (goTemplate)
	{
		root = new GameObject(nullptr);
		*root = *goTemplate;
		root->ReGenerate();
		root->prefab = this;
	}
}

void ResourcePrefab::SerializeToBuffer(char*& buffer, uint& size)
{
	std::vector<GameObject*> gameObject_s;
	std::vector<ComponentTransform*> transforms;
	std::vector<ComponentMesh*> meshes;
	std::vector <ComponentCamera*> cameras;
	std::vector<ComponentMaterial*> materials;
	std::vector<ComponentScript*> scripts;

	root->Decompose(gameObject_s, transforms, meshes, cameras, materials, scripts, true);

	uint gameObjectsSize = 0u;
	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		gameObjectsSize += gameObject_s[i]->bytesToSerialize();
	}

	size = sizeof(uint) + gameObjectsSize + //Each gameobject's name has a different name lenght
		sizeof(uint) + ComponentTransform::bytesToSerialize() * transforms.size() +
		sizeof(uint) + ComponentMesh::bytesToSerialize() * meshes.size() +
		sizeof(uint) + ComponentCamera::bytesToSerialize() * cameras.size() +
		sizeof(uint) + ComponentMaterial::bytesToSerialize() * materials.size() +
		sizeof(uint) + ComponentScript::bytesToSerialize() * scripts.size();


	buffer = new char[size];
	char* cursor = buffer;

	uint numGOs = gameObject_s.size();

	uint bytes = sizeof(uint);
	memcpy(cursor, &numGOs, bytes);
	cursor += bytes;

	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		gameObject_s[i]->Serialize(cursor);
	}

	uint numTransforms = transforms.size();

	bytes = sizeof(uint);
	memcpy(cursor, &numTransforms, bytes);
	cursor += bytes;

	for (int i = 0; i < transforms.size(); ++i)
	{
		transforms[i]->Serialize(cursor);
	}

	uint numMeshes = meshes.size();
	bytes = sizeof(uint);

	memcpy(cursor, &numMeshes, bytes);
	cursor += bytes;

	for (int i = 0; i < numMeshes; ++i)
	{
		meshes[i]->Serialize(cursor);
	}

	uint numCameras = cameras.size();
	bytes = sizeof(uint);
	memcpy(cursor, &numCameras, bytes);
	cursor += bytes;

	for (int i = 0; i < numCameras; ++i)
	{
		cameras[i]->Serialize(cursor);
	}

	uint numMaterials = materials.size();
	bytes = sizeof(uint);

	memcpy(cursor, &numMaterials, bytes);
	cursor += bytes;

	for (int i = 0; i < numMaterials; ++i)
	{
		materials[i]->Serialize(cursor);
	}

	uint numScripts = scripts.size();
	bytes = sizeof(uint);

	memcpy(cursor, &numScripts, bytes);
	cursor += bytes;

	for (int i = 0; i < scripts.size(); ++i)
	{
		scripts[i]->Serialize(cursor);
	}
}

void ResourcePrefab::DeSerializeFromBuffer(char*& buffer)
{
	char* cursor = buffer;

	std::vector<GameObject*> gameObject_s;
	std::vector<ComponentTransform*> transforms;
	std::vector<ComponentMesh*> meshes;
	std::vector <ComponentCamera*> cameras;
	std::vector<ComponentMaterial*> materials;
	std::vector<ComponentScript*> scripts;

	uint numGOs = 0;
	uint bytes = sizeof(uint);

	memcpy(&numGOs, cursor, bytes);
	cursor += bytes;

	std::vector<uint32_t> parentUUIDs;
	for (int i = 0; i < numGOs; ++i)
	{
		GameObject* newGO = new GameObject(nullptr);
		uint32_t parentUUID;
		newGO->DeSerialize(cursor, parentUUID);
		parentUUIDs.push_back(parentUUID);
		gameObject_s.push_back(newGO);
	}

	for (int i = 0; i < gameObject_s.size(); ++i) //For each gameobject
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (parentUUIDs[i] == gameObject_s[j]->uuid) //Look for a gameobject whose UUID is the same as your parent
			{
				gameObject_s[i]->parent = gameObject_s[j]; //Create this parent-child relationship
				gameObject_s[j]->AddChild(gameObject_s[i]);
			}
		}
	}

	uint numTransforms = 0u;
	bytes = sizeof(uint);
	memcpy(&numTransforms, cursor, bytes);
	cursor += bytes;

	std::vector<uint32_t> goUUIDs;

	for (int i = 0; i < numTransforms; ++i)
	{
		ComponentTransform* newTransform = new ComponentTransform();
		uint32_t goUUID;
		newTransform->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		transforms.push_back(newTransform);
	}

	for (int i = 0; i < transforms.size(); ++i) //For each transform
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid) //Look for a gameobject whose UUID is the same as your one
			{
				transforms[i]->gameObject = gameObject_s[j]; //Create this gameobject-component relationship
				gameObject_s[j]->AddComponent(transforms[i]);
				gameObject_s[j]->transform = transforms[i];
			}
		}
	}

	uint numMeshes = 0u;
	bytes = sizeof(uint);

	memcpy(&numMeshes, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numMeshes; ++i)
	{
		ComponentMesh* newMesh = new ComponentMesh(nullptr);
		uint32_t goUUID;
		newMesh->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		meshes.push_back(newMesh);
	}

	for (int i = 0; i < meshes.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				meshes[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(meshes[i]);
			}
		}
	}

	uint numCameras = 0u;
	bytes = sizeof(uint);
	memcpy(&numCameras, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numCameras; ++i)
	{
		ComponentCamera* newCamera = new ComponentCamera(nullptr);
		uint32_t goUUID;
		newCamera->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		cameras.push_back(newCamera);
	}

	for (int i = 0; i < cameras.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				cameras[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(cameras[i]);
				cameras[i]->updateFrustum();
			}
		}
	}

	uint numMaterials = 0u;
	bytes = sizeof(uint);

	memcpy(&numMaterials, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numMaterials; ++i)
	{
		ComponentMaterial* newMaterial = new ComponentMaterial(nullptr);
		uint32_t goUUID;
		newMaterial->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		materials.push_back(newMaterial);
	}

	for (int i = 0; i < materials.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				materials[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(materials[i]);
			}
		}
	}

	uint numScripts = 0u;
	bytes = sizeof(uint);

	memcpy(&numScripts, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numScripts; ++i)
	{
		ComponentScript* newScript = new ComponentScript("");
		uint32_t goUUID;
		newScript->deSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		scripts.push_back(newScript);
	}

	for (int i = 0; i < scripts.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				scripts[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(scripts[i]);
			}
		}
	}

	std::vector<GameObject*> roots;
	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		if (gameObject_s[i]->parent == nullptr)
		{
			roots.push_back(gameObject_s[i]);
		}
	}

	Debug.LogWarning("Multiple roots found inside a serialized buffer. Some data will be missing.");

	root = roots[0];
	roots[0]->initAABB();
	roots[0]->transformAABB();

	for (int i = 1; i < roots.size(); ++i)
	{
		delete roots[i];
	}
	roots.clear();

	root->prefab = this;
}
