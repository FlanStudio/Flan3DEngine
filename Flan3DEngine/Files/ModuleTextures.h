#pragma once

#include "Module.h"
#include "Globals.h"
#include <vector>

struct Texture
{
	char* name;

	unsigned char* data = nullptr;

	//OpenGL id
	uint id = 0;

	uint width = 0;
	uint height = 0;

	~Texture();
};




class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true) : Module("TextureLoader", start_enabled) {}
	virtual ~ModuleTextures();

	bool Init();

	bool Start();

	bool CleanUp()
	{
		return true;
	}

	update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}
	update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}
	update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const { return true; }

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj) { return true; }

	//returns the index of the image loaded
	uint LoadTexture(char* file, bool useFileSystem = true);

	void getTextureSize(uint id, uint& width, uint& size) const;

	void deleteTextures();

	void guiTextures() const;

public:
	std::vector<Texture*> textures;
};