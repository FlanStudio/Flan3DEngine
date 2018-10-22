#include "Application.h"
#include "ModuleFileSystem.h"
#include "PhysFS/physfs.h"
#pragma comment (lib, "PhysFS/physfs.lib")

#include <bitset>

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module("ModuleFileSystem", start_enabled) {}
ModuleFileSystem::~ModuleFileSystem() {}

bool ModuleFileSystem::Init()
{
	//Initialize PhysFS library
	PHYSFS_init(nullptr);

	//Add root directory (Read-Only)
	AddPath(".");

	//Read/Write directory
	AddPath("./Assets/", "Assets");
	setWriteDir("./Assets");
	AddPath("./Assets/config/", "config");


	//NOTE: We are not using a .zip because of .zip's are Read-Only in PHYSFS and it's directories are not mountable.

	return true;
}

bool ModuleFileSystem::Start()
{
	return true;
}

bool ModuleFileSystem::CleanUp()
{
	bool ret = PHYSFS_deinit() != 0;

	if (ret == false)
	{
		Debug.LogError("FILESYSTEM: Could not close PhysFS. Error: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return ret;
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
		Debug.LogError("FILESYSTEM: Failed adding \"%s\" to the search path. Error: \"%s\"", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
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
		Debug.LogError("FILESYSTEM: Write dir couln't be set to \"%s\". Error: \"%s\"", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		ret = false;
	}

	return ret;
}

bool ModuleFileSystem::OpenRead(char* file, char** buffer, int& size) const
{
	bool ret = true;
	PHYSFS_File* FSfile = PHYSFS_openRead(file);
	if (!FSfile)
	{
		Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		ret = false;
	}
	else
	{
		size = PHYSFS_fileLength(FSfile);
		if (size == -1)
		{
			Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			ret = false;
		}
		
		else
		{
			*buffer = new char[size];
			int readed = PHYSFS_readBytes(FSfile, *buffer, size);
			if (readed == -1)
			{
				Debug.LogError("FILESYSTEM: File \"%s\" couln't be read. Error: \"%s\"", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				*buffer = nullptr;
				size = 0;
				ret = false;
			}
		}
		
		//Close the PHYSFS_File* opened
		if (PHYSFS_close(FSfile) == 0)
		{
			Debug.LogError("FILESYSTEM: Couldn't close the PHYSFS_File opened. Error: \"%s\"", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}
	
	if(ret)
		Debug.Log("FILESYSTEM: \"%s\" succesfully read", file);

	return ret;
}

bool ModuleFileSystem::OpenWrite(char* file, char* buffer)
{
	bool ret = true;
	PHYSFS_File* FSFile = PHYSFS_openWrite(file);
	if (!FSFile)
	{
		Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		ret = false;
	}
	else
	{
		if (!buffer)
		{
			Debug.LogError("FILESYSTEM: Buffer is empty");
			ret = false;
		}
		else
		{
			int size = strlen(buffer);
			int written = PHYSFS_writeBytes(FSFile, buffer, size);
			if (written == -1)
			{
				Debug.LogError("FILESYSTEM: File \"%s\" couln't be written. Error: \"%s\"", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				return false;
			}
		}
		
		//Close the PHYSFS_File* opened
		if (PHYSFS_close(FSFile) == 0)
		{
			Debug.LogError("FILESYSTEM: Couldn't close the PHYSFS_File opened. Error: \"%s\"", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}
	
	if(ret)
		Debug.Log("FILESYSTEM: \"%s\" succesfully written", file);

	return ret;
}

char* ModuleFileSystem::ASCII_TO_BINARY(char* ascii_string)
{
	std::string ascii(ascii_string);
	std::string output;
	for (std::size_t i = 0; i < ascii.size(); ++i)
	{
		output += std::bitset<8>(ascii.c_str()[i]).to_string();
	}
	
	char* ret = new char[output.size()];
	strcpy(ret, output.c_str());
	return ret;
}

char* ModuleFileSystem::BINARY_TO_ASCII(char* binary_string)
{
	std::stringstream ostring(binary_string);
	std::string output;
	while (ostring.good())
	{
		std::bitset<8> bits;
		ostring >> bits;
		char c = char(bits.to_ulong());
		output += c;
	}
	char* ret = new char[output.size()];
	strcpy(ret, output.c_str());
	return ret;
}