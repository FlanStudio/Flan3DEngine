#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

class ResourceTexture;

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

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const { return true; }

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj) { return true; }

	void getTextureSize(uint id, uint& width, uint& size) const;

	void deleteTextures();

	void guiTextures() const;

	bool isSupported(const std::string& extension) const;

	ResourceTexture* ExportResource(std::string file);

public:
	std::vector<ResourceTexture*> textures;
};

#endif