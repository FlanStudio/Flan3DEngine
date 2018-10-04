#include "FBXLoader.h"
#include "Application.h"

#include "assimp/include/scene.h"
#include "assimp/include/postprocess.h"
#include "assimp/include/cfileio.h"

#include <string>

#pragma comment (lib, "Assimp/libx86/assimp-vc140-mt.lib")


void LogCallback(const char* message, char* user)
{
	std::string log(message);
	if(log.find("Error") != std::string::npos)
		Debug.LogError("Assimp Error: %s", message);
	else
		Debug.Log("Assimp Log: %s", message);
}

bool FBXLoader::Start()
{
	meshes.reserve(maxMeshes);

	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool FBXLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

update_status FBXLoader::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXLoader::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXLoader::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

//Save changes the JSON, not the module
bool FBXLoader::Save(JSON_Object* obj) const
{
	return true;
}

//Load changes the module, not the JSON
bool FBXLoader::Load(const JSON_Object* obj)
{
	return true;
}

void FBXLoader::LoadFBX(char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Mesh* mymesh = new Mesh();
			aiMesh* mesh = scene->mMeshes[i];
			
			mymesh->num_vertex = mesh->mNumVertices;
			mymesh->vertex = new float[mesh->mNumVertices * 3];
			memcpy(mymesh->vertex, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3);
			meshes.push_back(mymesh);
			Debug.Log("New mesh loaded with &d vertices", mymesh->num_vertex);
		}


		aiReleaseImport(scene);
	}
	else
	{
		Debug.LogError("Error loading the scene %s. Error: %s", path, aiGetErrorString());
	}


}