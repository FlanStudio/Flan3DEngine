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

	//TIP: Dont forget to delete the buffer
	bool OpenRead(char* file, char** buffer, int& size) const;

	bool OpenWrite(char* file, char* buffer);

};