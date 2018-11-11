#ifndef __FBXExporter_H__
#define __FBXExporter_H__

#include "Module.h"
#include "Globals.h"

#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

#include "assimp/include/cimport.h"
#include <vector>

class aiNode;
class aiMesh;
class ComponentMesh;
class GameObject;
class aiScene;
class Resource;
class ResourceMesh;

class FBXExporter : public Module
{
public:
	FBXExporter(bool start_enabled = true) : Module("FBXExporter", start_enabled) { }
	~FBXExporter() {}

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const;

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj);

public:
	bool LoadFBX(char* path, bool useFS = true);
	std::vector<Resource*> ExportFBX(const std::string& file) const;

private:
	bool RecursivelyHierarchy(const aiNode* parent, const GameObject* parentGO, const aiScene* scene);
	bool FillMeshData(ComponentMesh* mymesh, aiMesh* mesh);
	std::vector<const aiNode*> decomposeAssimpHierarchy(const aiNode* rootNode) const;

private:
	aiLogStream stream;
};
void LogCallback(const char*, char*);

#endif
