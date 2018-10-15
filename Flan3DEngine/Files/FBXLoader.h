#pragma once

#include "Module.h"
#include "Globals.h"

#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

#include "assimp/include/cimport.h"
#include <vector>

class aiNode;
class aiMesh;
class MeshComponent;
class GameObject;
class aiScene;

class FBXLoader : public Module
{
public:
	FBXLoader(bool start_enabled = true) : Module("FBXLoader", start_enabled) { }
	~FBXLoader() {}

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const;

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj);

public:
	bool LoadFBX(char* path, bool useFS = true);

private:
	bool RecursivelyHierarchy(const aiNode* parent, const GameObject* parentGO, const aiScene* scene);
	bool FillMeshData(MeshComponent* mymesh, aiMesh* mesh);

private:
	aiLogStream stream;
};
void LogCallback(const char*, char*);

