#include "Application.h"
#include "ModuleFileSystem.h"
#include "PhysFS/physfs.h"
#pragma comment (lib, "PhysFS/physfs.lib")

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module(start_enabled) {}
ModuleFileSystem::~ModuleFileSystem() {}

bool ModuleFileSystem::Init()
{
	//Initialize PhysFS library
	PHYSFS_init(nullptr);

	//Add root directory (Read-Only)
	AddPath(".");

	//Read/Write directory
	AddPath("./Assets", "Assets");
	setWriteDir("./Assets");

	//NOTE: We are not using a .zip because of .zip's are Read-Only in PHYSFS and it's directories are not mountable.

	return true;
}

bool ModuleFileSystem::Start()
{
	return true;
}

bool ModuleFileSystem::CleanUp()
{
	return true;
}

bool ModuleFileSystem::AddPath(char* path, char* mount)
{
	bool ret = true;
	if (PHYSFS_mount(path, mount, 1) != 0)
	{
		Debug.Log("FILESYSTEM: Added \"%s\" to the search path", path);
		if (mount != "")
			Debug.Log("FILESYSTEM: Mounted \"%s\" into \"%s\"", path, mount);
	}
	else
	{
		ret = false;
		Debug.LogError("FILESYSTEM: Failed adding \"%s\" to the search path. Error: \"%s\"", path, PHYSFS_getLastError());
	}
	return ret;
}

bool ModuleFileSystem::setWriteDir(char* path)
{
	bool ret = true;

	if (PHYSFS_setWriteDir(path) != 0)
	{
		Debug.Log("FILESYSTEM: Write dir correctly set to \"%s\"", path);
	}
	else
	{
		Debug.LogError("FILESYSTEM: Write dir couln't be set to \"%s\". Error: \"%s\"", path, PHYSFS_getLastError());
		ret = false;
	}

	return ret;
}