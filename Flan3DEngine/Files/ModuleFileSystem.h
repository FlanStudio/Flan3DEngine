#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#define ASSETS_FOLDER "Assets/"
#define LIBRARY_FOLDER "Library/"
#define CONFIG_FOLDER "Library/config/"

#define SCENES_ASSETS_FOLDER "Assets/Scenes/"
#define SCENES_EXTENSION ".flanScene"

#include <vector>
#include <string>
#include "Module.h"
#include "Timer.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	virtual ~ModuleFileSystem();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	bool CleanUp();

	bool AddPath(char* path, char* mount = "");
	bool setWriteDir(char* path);

	//WARNING: Don't forget to delete the buffer
	bool OpenRead(std::string file, char** buffer, int& size) const;
	
	bool OpenWrite(std::string file, char* buffer);
	bool OpenWriteBuffer(std::string file, void* buffer, uint size);
	
	//WARNING: Don't forget to delete the buffer
	char* ASCII_TO_BINARY(char* ascii_string);

	//WARNING: Don't forget to delete the buffer
	char* BINARY_TO_ASCII(char* binary_string);

	//WARNING: Don't forget to delete the Directory file after use
	Directory getDirFiles(char* dir);

	void fileSystemGUI();

	bool Exists(std::string file) const;

private:
	void recursiveDirectory(Directory& directory);

public:
	Directory AssetsDirSystem;

private:
	float updateAssetsCounter = 0.0f;
	float updateAssetsRate = 1.0f;
};
#endif