#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#define ASSETS_FOLDER "Assets/"
#define LIBRARY_FOLDER "Library/"
#define SETTINGS_FOLDER "Settings/"

#define SCENES_ASSETS_FOLDER "Assets/Scenes/"
#define SCENES_EXTENSION ".flanScene"

#define PREFABS_FOLDER "Assets/Prefabs"
#define PREFABS_EXTENSION ".flanPrefab"

#define TEXTURES_LIBRARY_FOLDER "Library/Textures"
#define TEXTURES_EXTENSION ".dds"

#define MESHES_LIBRARY_FOLDER "Library/Meshes"
#define MESHES_EXTENSION ".flanMesh"

#include <vector>
#include <string>
#include "Module.h"
#include "Timer.h"

#include "Directory.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	virtual ~ModuleFileSystem();

	bool Init();
	bool Start();
	update_status PreUpdate();
	bool CleanUp();

	bool AddPath(char* path, char* mount = "");
	bool setWriteDir(char* path);

	//WARNING: Don't forget to delete the buffer
	bool OpenRead(std::string file, char** buffer, int& size) const;
	
	bool OpenWrite(std::string file, char* buffer);
	bool OpenWriteBuffer(std::string file, void* buffer, uint size);
	
	bool CopyExternalFileInto(const std::string& file, const std::string& newLocation);
	bool MoveFileInto(const std::string& file, const std::string& newLocation);

	//WARNING: Don't forget to delete the buffer
	char* ASCII_TO_BINARY(char* ascii_string);

	//WARNING: Don't forget to delete the buffer
	char* BINARY_TO_ASCII(char* binary_string);

	void fileSystemGUI();

	bool Exists(std::string file) const;

	//Fill a vector with all the files in Assets, with their respective path
	void getFilesPath(std::vector<std::string>& files) const;

	//Extracts the extension from a string. 
	// return "" if not founded
	std::string getExt(const std::string& file) const;

	bool deleteDirectory(const std::string& directory) const;
	bool emptyDirectory(const std::string& directory) const;
	bool deleteFiles(const std::string& rootDirectory, const std::string& extension) const;
	bool deleteFile(const std::string& filePath) const;

	void BeginTempException(std::string directory);
	void EndTempException();

	void UpdateAssetsDir();
	void saveAssetsState();

private:
	//WARNING: Don't forget to delete the Directory file after use
	Directory getDirFiles(char* dir) const;

	void recursiveDirectory(Directory& directory);
	void SendEvents(const Directory& newAssetsDir);

private:
	Directory AssetsDirSystem;

	float updateAssetsCounter = 0.0f;
	float updateAssetsRate = 1.0f;

	std::string tempException;
};
#endif