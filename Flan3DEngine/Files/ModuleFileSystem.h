#include "Module.h"

#pragma once

#define ASSETS_FOLDER "Assets"
#define LIBRARY_FOLDER "Library"

#include <vector>

struct Directory
{
	char* name;
	std::vector<char*> files;
	std::vector<Directory> directories;
	~Directory()
	{
		delete[] name;
		name = nullptr;

		for (int i = 0; i < files.size(); ++i)
		{
			delete[] files[i];
			files[i] = nullptr;
		}
		files.clear();

		for (int i = 0; i < directories.size(); ++i)
		{
			directories[i].~Directory();
		}
		directories.clear();
	}
};

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	virtual ~ModuleFileSystem();

	bool Init();
	bool Start();
	bool CleanUp();

	bool AddPath(char* path, char* mount = "");
	bool setWriteDir(char* path);

	//WARNING: Don't forget to delete the buffer
	bool OpenRead(char* file, char** buffer, int& size) const;

	bool OpenWrite(char* file, char* buffer);
	
	//WARNING: Don't forget to delete the buffer
	char* ASCII_TO_BINARY(char* ascii_string);

	//WARNING: Don't forget to delete the buffer
	char* BINARY_TO_ASCII(char* binary_string);

	Directory getDirFiles(char* dir);
};