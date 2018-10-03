#include "FBXLoader.h"
#include "Assimp/include/mesh.h"


#pragma comment (lib, "Assimp/libx86/assimp-vc140-mt.lib")

bool FBXLoader::Start()
{
	return true;
}

bool FBXLoader::CleanUp()
{
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