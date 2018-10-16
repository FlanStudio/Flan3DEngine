#include "Module.h"

#pragma once

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
};