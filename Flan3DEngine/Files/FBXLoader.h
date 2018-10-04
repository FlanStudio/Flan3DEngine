#pragma once

#include "Module.h"

#include "assimp/include/cimport.h"


struct VertexData
{
	uint id_index = 0;			// index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0;			// unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;
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

	void LoadFBX(char* path);

private:
	aiLogStream stream;

};
void Test(const char*, char*);

