#include "Application.h"
#include "ModuleFileSystem.h"
#include "PhysFS/physfs.h"

#include "Brofiler\Brofiler.h"

#pragma comment (lib, "PhysFS/physfs.lib")

#include <bitset>

#include "imgui/imgui.h"

#include "Resource.h"

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module("ModuleFileSystem", start_enabled) {}
ModuleFileSystem::~ModuleFileSystem() {}

#define SHOWMETAFILES false

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
	AddPath("./Settings/", "Settings");

	//Internal Directory: Engine textures
	AddPath("./internal.f", "Internal");

	//User-Dependent Directory
	AddPath((char*)PHYSFS_getPrefDir(App->organization.data(), App->engineName.data()), "Saves");

	//NOTE: We are not using a .zip because of .zip's are Read-Only in PHYSFS and it's directories are not mountable.
	
	//Check if Assets has changed since the last time the engine was closed

	char* buffer;
	int size;

	if (!Exists("Library/Meshes")) //If you have library empty then rexport everything
	{
		std::string prevWriteDir = PHYSFS_getWriteDir();

		std::string userPath = PHYSFS_getPrefDir(App->organization.data(), App->engineName.data());
		setWriteDir((char*)userPath.data());

		deleteFile("lastAssetsState.fl");

		setWriteDir((char*)prevWriteDir.data());
	}		

	if(Exists("Saves/lastAssetsState.fl"))	
	{
		if(OpenRead("Saves/lastAssetsState.fl", &buffer, size))
		{
			Directory newDirectory;
			char* cursor = buffer;
			AssetsDirSystem.DeSerialize(cursor);

			newDirectory = getDirFiles("Assets");

			if (newDirectory != AssetsDirSystem)
			{
				SendEvents(newDirectory);
			}

			delete buffer;
		}		
	}
	else
	{		
		emptyDirectory("Library");
		deleteFiles("Assets", ".meta");
		AssetsDirSystem = getDirFiles("Assets");
	}

	return true;
}

bool ModuleFileSystem::Start()
{
	return true;
}

bool ModuleFileSystem::CleanUp()
{

	saveAssetsState();

	bool ret = PHYSFS_deinit() != 0;

	if (ret == false)
	{
		Debug.LogError("FILESYSTEM: Could not close PhysFS. Error: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}

	return ret;
}

update_status ModuleFileSystem::PreUpdate()
{
	BROFILER_CATEGORY("FileSystemPreUpdate", Profiler::Color::Azure)

	updateAssetsCounter += App->time->dt;
	if (updateAssetsCounter >= 1.0f / updateAssetsRate)
	{
		updateAssetsCounter = 0.0f;
		UpdateAssetsDir();
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

bool ModuleFileSystem::CopyExternalFileInto(const std::string& file, const std::string& newLocation)
{
	uint fileNamePos = file.find_last_of("/");
	std::string fileName = file.substr(fileNamePos+1);

	char* buffer;
	int size;
	if (!OpenRead("Exception/" + fileName, &buffer, size))
	{
		return false;
	}

	if (!OpenWriteBuffer(newLocation + fileName, buffer, size))
	{
		return false;
	}

	delete buffer;

	return true;
}

bool ModuleFileSystem::MoveFileInto(const std::string& file, const std::string& newLocation)
{
	char* buffer;
	int size;
	if (!OpenRead(file, &buffer, size))
	{
		return false;
	}

	if (!OpenWriteBuffer(newLocation, buffer, size))
	{
		delete buffer;
		return false;
	}

	if (!deleteFile(file))
	{
		delete buffer;
		return false;
	}

	delete buffer;
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

Directory ModuleFileSystem::getDirFiles(char* dir) const 
{
	Directory ret;
	ret.fullPath = dir;
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
		std::string fulldir(dir + std::string("/") + std::string(files[i]));
		std::string file(files[i]);
		if(file.find(".") == std::string::npos) //Is a directory, have not extension
		{							
			Directory child = getDirFiles((char*)fulldir.data());
			child.fullPath = fulldir;
			ret.directories.push_back(child);
		}
		else
		{
			PHYSFS_Stat stats;
			PHYSFS_stat(fulldir.data(), &stats);
			File file;
			file.lastModTime = stats.modtime; //Save the last modification time in order to know when a file has changed
			file.name = files[i];
			ret.files.push_back(file);
		}
	}
	PHYSFS_freeList(files);

	return ret;
}

void ModuleFileSystem::fileSystemGUI()
{
	recursiveDirectory(AssetsDirSystem);
}

bool ModuleFileSystem::Exists(std::string file) const
{
	return PHYSFS_exists(file.data());
}

void ModuleFileSystem::getFilesPath(std::vector<std::string>& files) const
{
	AssetsDirSystem.getFullPaths(files);
}

std::string ModuleFileSystem::getExt(const std::string& file) const
{
	std::string ret;

	uint pos = file.find_last_of(".");
	ret = file.substr(pos);

	return ret;
}

bool ModuleFileSystem::deleteDirectory(const std::string & directory) const
{
	emptyDirectory(directory);
	PHYSFS_delete(directory.data());
	return true;
}

bool ModuleFileSystem::emptyDirectory(const std::string& dir) const
{
	Directory directory = getDirFiles((char*)dir.c_str());

	for(int i = 0; i < directory.files.size(); ++i)
	{
		PHYSFS_delete(std::string(directory.fullPath + "/" + directory.files[i].name).c_str());
	}

	for (int i = 0; i < directory.directories.size(); ++i)
	{
		emptyDirectory(directory.directories[i].fullPath);
		deleteDirectory(directory.directories[i].fullPath);
	}	

	return true;
}

bool ModuleFileSystem::deleteFiles(const std::string& root, const std::string& extension) const
{
	Directory directory = getDirFiles((char*)root.c_str());

	for (int i = 0; i < directory.files.size(); ++i)
	{
		if(getExt(directory.files[i].name) == extension)
			PHYSFS_delete(std::string(directory.fullPath + "/" + directory.files[i].name).c_str());
	}

	for (int i = 0; i < directory.directories.size(); ++i)
	{
		deleteFiles(directory.directories[i].fullPath, extension);
	}

	return true;
}

bool ModuleFileSystem::deleteFile(const std::string& filePath) const
{
	return PHYSFS_delete(filePath.c_str()) != 0;
}

void ModuleFileSystem::BeginTempException(std::string directory)
{
	if (tempException.empty())
	{
		tempException = directory;

		//Add to the search path
		if (PHYSFS_mount(directory.data(), "Exception", 1) == 0)
		{
			char* error = (char*)PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		}
	}
	else
	{
		Debug.LogError("FILESYSTEM: YOU CAN ONLY HAVE 1 EXCEPTION LOADED AT THE SAME TIME");
	}
}

void ModuleFileSystem::EndTempException()
{
	PHYSFS_unmount(tempException.data());
	tempException.clear();
}

void ModuleFileSystem::UpdateAssetsDir()
{
	Directory newAssetsDir = getDirFiles("Assets");

	if (newAssetsDir != AssetsDirSystem)
	{
		SendEvents(newAssetsDir);
		AssetsDirSystem = newAssetsDir;
	}
}

void ModuleFileSystem::saveAssetsState()
{
	//We cannot update the assets dir before saving because of no one would receive events after cleanup

	uint fileSize = AssetsDirSystem.bytesToSerialize();
	char* buffer = new char[fileSize];
	char* cursor = buffer;

	AssetsDirSystem.Serialize(cursor);

	std::string prevWriteDir = PHYSFS_getWriteDir();

	std::string userPath = PHYSFS_getPrefDir(App->organization.data(), App->engineName.data());
	setWriteDir((char*)userPath.data());
	
	App->fs->OpenWriteBuffer("lastAssetsState.fl", buffer, fileSize);

	setWriteDir((char*)prevWriteDir.data());
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
			if (getExt(directory.files[i].name) == ".meta" && !SHOWMETAFILES)
				continue;

			ImGuiTreeNodeFlags cflags = 0;
			cflags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
			bool clicked = ImGui::TreeNodeEx(directory.files[i].name.data(), cflags);
			
			//Starting the drag event	
			ImGuiDragDropFlags flags = 0;
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoPreviewTooltip; //This is to only let ImGui create the Tooltip if we are dragging a loaded resource
			if (ImGui::BeginDragDropSource(flags))
			{
				Resource* resource = App->resources->FindByFile((char*)std::string(directory.fullPath + "/" + directory.files[i].name).data());	
				std::string ext = getExt(directory.files[i].name);
				if (resource)
				{
					ImGui::SetDragDropPayload("DraggingResources", &resource, resource->getBytes());
					ImGui::BeginTooltip();
					ImGui::Text(resource->getFile());
					ImGui::EndTooltip();
				}			
				else if(ext == ".FBX" || ext == ".fbx")
				{
					//Manage FBX drag and drop here
					std::string fullAddress = directory.fullPath + "/" + directory.files[i].name;
					ImGui::SetDragDropPayload("DraggingFBX", (char*)fullAddress.c_str(), fullAddress.size()+1);
					ImGui::BeginTooltip();
					ImGui::Text(directory.files[i].name.data());
					ImGui::EndTooltip();
				}
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemClicked(0))
			{
				int pos = directory.files[i].name.find_last_of(".");
				std::string extension = directory.files[i].name.substr(pos);
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
				std::string file = directory.files[i].name.substr(0, directory.files[i].name.find_last_of("."));
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

void ModuleFileSystem::SendEvents(const Directory& newAssetsDir)
{
	//To get the moved elements, initialize here 2 vectors with both fullpaths. Below here when a file is detected as added or deleted, 
	//delete their reference in both fullpaths. At the end of the added and deleted fors, compare both fullpaths. 
	//The different ones have to have been moved.

	//Two arrays with a complete path for each file contained in the directory
	std::vector<std::string> newFullPaths;
	std::vector<std::string> oldFullPaths;
	newAssetsDir.getFullPaths(newFullPaths);
	AssetsDirSystem.getFullPaths(oldFullPaths);

	//Get the file names, without the path, for each file contained in the directory
	std::vector<File> newFiles;
	std::vector<File> oldFiles;
	AssetsDirSystem.getFiles(oldFiles);
	newAssetsDir.getFiles(newFiles);

	//Check for deleted files
	for (int i = 0; i < oldFiles.size(); ++i)
	{
		bool exists = false;
		bool modified = false;
		for (int j = 0; j < newFiles.size(); ++j)
		{
			if (oldFiles[i] == newFiles[j])
				exists = true;
			else
				if (oldFiles[i].name == newFiles[j].name) //Exists, but the file has been modified
				{
					exists = true;
					modified = true;
				}
		}
		if (!exists)
		{
			//A file has been deleted, get the full path and send the event.
			//Delete this file from the oldFullPaths vector.

			std::string file = oldFiles[i].name;
			std::string fullPathFile;
			for (int j = 0; j < oldFullPaths.size(); ++j)
			{
				if (oldFullPaths[j].find(file) != std::string::npos)
				{
					//We founded the path
					fullPathFile = oldFullPaths[j];
					oldFullPaths.erase(oldFullPaths.begin() + j);
					break;
				}
			}
			Event event;
			event.fileEvent.type = EventType::FILE_DELETED;
			event.fileEvent.file = new char[fullPathFile.size() + 1];
			strcpy((char*)event.fileEvent.file, fullPathFile.c_str());
			App->SendEvent(event);
		}
		if (modified)
		{
			//Send the event
			std::string file = oldFiles[i].name;
			std::string fullPathFile;
			for (int j = 0; j < oldFullPaths.size(); ++j)
			{
				if (oldFullPaths[j].find(file) != std::string::npos)
				{
					//We founded the path. Do not erase from the vector
					fullPathFile = oldFullPaths[j];
					break;
				}
			}
			Event event;
			event.fileEvent.type = EventType::FILE_MODIFIED;
			event.fileEvent.file = new char[fullPathFile.size() + 1];
			strcpy((char*)event.fileEvent.file, fullPathFile.c_str());
			App->SendEvent(event);
		}
	}

	//Check for added files
	for (int i = 0; i < newFiles.size(); ++i)
	{
		bool existed = false;
		bool modified = false;
		for (int j = 0; j < oldFiles.size(); ++j)
		{
			if (newFiles[i].name == oldFiles[j].name) //We already checked and sent events for file modifications above.
				existed = true;
		}
		if (!existed)
		{
			//A file has been added, get the full path and send the event
			//Delete this file from the newFullPaths vector.

			std::string file = newFiles[i].name;
			std::string fullPathFile;
			for (int j = 0; j < newFullPaths.size(); ++j)
			{
				if (newFullPaths[j].find(file) != std::string::npos)
				{
					//We founded the path
					fullPathFile = newFullPaths[j];
					newFullPaths.erase(newFullPaths.begin() + j);
					break;
				}
			}
			Event event;
			event.fileEvent.type = EventType::FILE_CREATED;
			event.fileEvent.file = new char[fullPathFile.size() + 1];
			strcpy((char*)event.fileEvent.file, fullPathFile.c_str());
			App->SendEvent(event);
		}
	}

	//Now we only have complete paths referencing the same files in the FullPath vectors. 
	//Compare same-named files' paths and send events if they are different.
	for (int i = 0; i < oldFullPaths.size(); ++i)
	{
		int oldPos = oldFullPaths[i].find_last_of("/") + 1;
		std::string oldfile = oldFullPaths[i].substr(oldPos);

		for (int j = 0; j < newFullPaths.size(); ++j)
		{
			int newPos = newFullPaths[j].find_last_of("/") + 1;
			std::string newfile = newFullPaths[j].substr(newPos);

			if (oldfile != newfile) //Different files, don't care their path, continue searching
				continue;

			std::string oldPath = oldFullPaths[i].substr(0, oldPos);
			std::string newPath = newFullPaths[j].substr(0, newPos);

			if (oldPath != newPath) //Same file, different paths
			{
				Event event;
				event.fileEvent.type = EventType::FILE_MOVED;
				event.fileEvent.file = new char[newFullPaths[j].size() + 1];
				strcpy((char*)event.fileEvent.file, newFullPaths[j].c_str());

				event.fileEvent.oldLocation = new char[oldFullPaths[i].size() + 1];
				strcpy((char*)event.fileEvent.oldLocation, oldFullPaths[i].c_str());
				App->SendEvent(event);
				break;
			}
		}
	}
}
