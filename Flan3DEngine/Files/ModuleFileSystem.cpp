#include "Application.h"
#include "ModuleFileSystem.h"
#include "PhysFS/physfs.h"
#pragma comment (lib, "PhysFS/physfs.lib")

#include <bitset>

#include "imgui/imgui.h"

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module("ModuleFileSystem", start_enabled) {}
ModuleFileSystem::~ModuleFileSystem() {}

bool ModuleFileSystem::Init()
{
	//Initialize PhysFS library
	PHYSFS_init(nullptr);

	//Write dir on "Game" folder in order to be able of exporting files to both "Assets" and "Library" folders.
	setWriteDir(".");

	//Add directories to the path
	AddPath(".");
	AddPath("./Assets/", "Assets");
	AddPath("./Library/", "Library");
	AddPath("./Library/config", "config");

	//NOTE: We are not using a .zip because of .zip's are Read-Only in PHYSFS and it's directories are not mountable.
	
	AssetsDirSystem = getDirFiles("Assets");

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

update_status ModuleFileSystem::PreUpdate(float dt)
{
	updateAssetsCounter += dt;
	if (updateAssetsCounter >= 1.0f / updateAssetsRate)
	{
		updateAssetsCounter = 0.0f;
		Directory newAssetsDir = getDirFiles("Assets");
		
		if (newAssetsDir != AssetsDirSystem)
		{
			//TODO: SEND EVENTS, ASSETS CHANGED.
			AssetsDirSystem = newAssetsDir;
		}		
	}
	return update_status::UPDATE_CONTINUE;
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
		if (PHYSFS_getLastErrorCode() == PHYSFS_ErrorCode::PHYSFS_ERR_NOT_FOUND) //If the directory is not found, create it and try to add it to the path again
		{
			std::string pathStr(path);
			std::string pathWithoutPoint = pathStr.substr(1, std::string::npos);

			bool error = PHYSFS_mkdir(pathWithoutPoint.c_str()) == 0;
			if (error)
			{
				Debug.LogError("FILESYSTEM: Could not create the directory %s. Error: %s", pathWithoutPoint.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				return false;
			}				
			return AddPath(path, mount);
		}
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

bool ModuleFileSystem::OpenRead(std::string file, char** buffer, int& size) const
{
	bool ret = true;
	PHYSFS_File* FSfile = PHYSFS_openRead(file.c_str());
	if (!FSfile)
	{
		Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		ret = false;
	}
	else
	{
		size = PHYSFS_fileLength(FSfile);
		if (size == -1)
		{
			Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			ret = false;
		}

		else
		{
			*buffer = new char[size];
			int readed = PHYSFS_readBytes(FSfile, *buffer, size);
			if (readed == -1)
			{
				Debug.LogError("FILESYSTEM: File \"%s\" couln't be read. Error: \"%s\"", file.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
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

	if (ret)
		Debug.Log("FILESYSTEM: \"%s\" succesfully read", file.c_str());

	return ret;
}

bool ModuleFileSystem::OpenWrite(std::string file, char* buffer)
{
	bool ret = true;
	PHYSFS_File* FSFile = PHYSFS_openWrite(file.c_str());
	if (!FSFile)
	{
		Debug.LogError("FILESYSTEM: File \"%s\" couln't be found. Error: \"%s\"", file.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
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
				Debug.LogError("FILESYSTEM: File \"%s\" couln't be written. Error: \"%s\"", file.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
				return false;
			}
		}

		//Close the PHYSFS_File* opened
		if (PHYSFS_close(FSFile) == 0)
		{
			Debug.LogError("FILESYSTEM: Couldn't close the PHYSFS_File opened. Error: \"%s\"", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}

	if (ret)
		Debug.Log("FILESYSTEM: \"%s\" succesfully written", file.c_str());

	return ret;
}

bool ModuleFileSystem::OpenWriteBuffer(std::string file, void* buffer, uint size)
{
	//Ensure the directory exists, this is not created automatically
	int pos = file.find_last_of("/");
	if (pos == std::string::npos)
		pos = file.find_last_of("\\");
	std::string directory = file.substr(0, pos + 1);
	PHYSFS_mkdir(directory.data());

	//Open/Create the file
	PHYSFS_File* PhysFile = PHYSFS_openWrite(file.data());
	if (!PhysFile)
	{
		Debug.LogError("FILESYSTEM: PhysFS could not load the file %s. Error: %s", file.data(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}

	int readed = PHYSFS_writeBytes(PhysFile, buffer, size);
	if (readed != size)
	{
		if (readed != -1)
		{
			Debug.LogWarning("FILESYSTEM: Not al bytes requestes could be readed. Size was %d and PhysFS readed only %d bytes", size, readed);
		}
		else
		{
			Debug.LogError("FILESYSTEM: Error writting on %s. Error: %s", file.data(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
			return false;
		}
	}

	PHYSFS_close(PhysFile);
	return true;
}

char* ModuleFileSystem::ASCII_TO_BINARY(char* ascii_string)
{
	std::string ascii(ascii_string);
	std::string output;
	for (std::size_t i = 0; i < ascii.size(); ++i)
	{
		output += std::bitset<8>(ascii.c_str()[i]).to_string();
	}
	
	char* ret = new char[output.size()+1];
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

Directory ModuleFileSystem::getDirFiles(char* dir)
{
	Directory ret;
	std::string dirstr(dir);
	std::string name;
	int pos = dirstr.find_last_of("/");
	if (pos != std::string::npos)
	{
		name = dirstr.substr(pos + 1, std::string::npos);
	}
	else
	{
		name = dirstr;
	}

	ret.name = name;

	char** files = PHYSFS_enumerateFiles(dir);
	for (int i = 0; files[i] != nullptr; ++i)
	{
		std::string file(files[i]);
		if(file.find(".") == std::string::npos) //Is a directory, have not extension
		{			
			std::string fulldir(dir + std::string("/") + std::string(files[i]));		
			Directory child = getDirFiles((char*)fulldir.data());
			child.fullPath = fulldir;
			ret.directories.push_back(child);
		}
		else
		{
			ret.files.push_back(files[i]);
		}
	}
	PHYSFS_freeList(files);

	return ret;
}

void ModuleFileSystem::fileSystemGUI()
{
	recursiveDirectory(AssetsDirSystem);
}

void ModuleFileSystem::recursiveDirectory(Directory& directory)
{
	ImGuiTreeNodeFlags flags = 0;
	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;

	if (ImGui::TreeNodeEx(directory.name.data(), flags))
	{
		for (int i = 0; i < directory.directories.size(); ++i)
		{
			recursiveDirectory(directory.directories[i]);
		}

		for (int i = 0; i < directory.files.size(); ++i)
		{
			ImGuiTreeNodeFlags cflags = 0;
			cflags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
			bool clicked = ImGui::TreeNodeEx(directory.files[i].data(), cflags);
			
			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemClicked(0))
			{
				int pos = directory.files[i].find_last_of(".");
				std::string extension = directory.files[i].substr(pos);
				if (extension == SCENES_EXTENSION)
				{				
					ImGui::OpenPopup("CAUTION");
				}
			}
			//TODO: Rename files and folders from assets window?

			ImGui::SetNextWindowSize(ImVec2(750, 160));		
			if (ImGui::BeginPopupModal("CAUTION", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove))
			{				
				ImVec2 windowPos = ImGui::GetWindowPos();
				std::string file = directory.files[i].substr(0, directory.files[i].find_last_of("."));
				ImVec2 cursorPos = ImGui::GetCursorScreenPos();
				ImGui::SetCursorScreenPos({ cursorPos.x + 20, cursorPos.y });
				ImGui::TextWrapped("All the not saved elements in hierarchy will be lost. Are you sure you want to load \"%s\"?", file.data());				
				ImGui::NewLine(); ImGui::NewLine();
				cursorPos = ImGui::GetCursorScreenPos();
				ImGui::SetCursorScreenPos({ windowPos.x + 750 / 2 - 250 / 2, cursorPos.y });
				if (ImGui::Button("LOAD", ImVec2(120, 40))) 
				{					
					App->scene->DeSerialize(directory.fullPath + "/" + file, SCENES_EXTENSION);
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::SameLine();
				if (ImGui::Button("CANCEL", ImVec2(120, 40))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
			if(clicked)
				ImGui::TreePop();

		}

		ImGui::TreePop();
	}
}