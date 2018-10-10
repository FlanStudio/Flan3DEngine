#pragma once

#include "Module.h"
#include "Globals.h"

#include "assimp/include/cimport.h"
#include <vector>

static const uint maxMeshes = 5;

struct Mesh
{
	uint index_ID = 0;			// index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint vertex_ID = 0;		// unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint normals_ID = 0;
	float* normals = nullptr;

	uint normalLines_ID = 0;
	float* normalLines = nullptr;

	uint colors_ID = 0;
	float* colors = nullptr;

	uint textureCoords_ID = 0;
	float* textureCoords = nullptr;

	~Mesh();
	void genBuffers();
	void destroyBuffers();
	void Draw();
	void genNormalLines();
	void UpdateNormalsLenght();
	void drawNormals();
};
	


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
	void deleteFBX(Mesh* mesh);
	void clearMeshes();
	void UpdateNormalsLenght();

public:
	bool drawNormals = false;
	float normalsLenght = 0.5f;

private:
	aiLogStream stream;
	std::vector<Mesh*> meshes;
};
void LogCallback(const char*, char*);

