#include "Application.h"
#include "ResourceScript.h"

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

ResourceScript::~ResourceScript()
{
	
}

void ResourceScript::SerializeToMeta(char*& cursor) const
{
	uint bytes = sizeof(UID);
	memcpy(cursor, &uuid, bytes);
	cursor += bytes;

	bytes = sizeof(ScriptState);
	memcpy(cursor, &state, bytes);
	cursor += bytes;
}

void ResourceScript::DeSerializeFromMeta(char*& cursor)
{
	uint bytes = sizeof(UID);
	memcpy(&uuid, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(ScriptState);
	memcpy(&state, cursor, bytes);
	cursor += bytes;
}

uint ResourceScript::bytesToSerializeMeta() const
{
	return sizeof(UID) + sizeof(ScriptState);
}

uint ResourceScript::getBytes() const
{
	return sizeof(ResourceScript);
}

bool ResourceScript::Compile()
{
	bool ret = true;

	if (assembly)
	{
		return true;
		mono_image_close(image);
		image = nullptr;
		mono_assembly_close(assembly);
		assembly = nullptr;
	}

	std::string goRoot(R"(cd\ )");
	std::string goMonoBin(" cd \"" + App->fs->getAppPath() + "\\Mono\\bin\"");

	std::string compileCommand(" mcs -target:library ");

	std::string fileName = file.substr(file.find_last_of("/") + 1);
	std::string windowsFormattedPath = pathToWindowsNotation(file);

	std::string path = std::string("\"" + std::string(App->fs->getAppPath())) + windowsFormattedPath + "\"";

	std::string error;

	if (!exec(std::string(goRoot + "&" + goMonoBin + "&" + compileCommand + path + " " + App->scripting->getReferencePath()).data(), error))
	{
		ret = false;
		if (!error.empty())
			Debug.LogError("Error compiling the script %s. Error: %s", fileName, error.data());
		else
			Debug.LogError("Error compiling the script %s.");
	}

	if (ret)
	{
		//Move the dll to the Library folder.
		std::string dllPath = file;
		dllPath = dllPath.substr(0, dllPath.find_last_of("."));
		dllPath += ".dll";

		std::string fileNameNoExt = file.substr(file.find_last_of("/") + 1);
		fileNameNoExt = fileNameNoExt.substr(0, fileNameNoExt.find("."));

		if (!App->fs->MoveFileInto(dllPath, ("Library/Scripts/" + fileNameNoExt + ".dll").data()))
			return false;
		
		exportedFile = "Library/Scripts/" + fileNameNoExt + ".dll";

		//Referencing the assembly from memory
		char* buffer;
		int size;
		if (!App->fs->OpenRead(exportedFile, &buffer, size))
			return false;

		//Loading assemblies from data instead of from file
		MonoImageOpenStatus status = MONO_IMAGE_ERROR_ERRNO;
		image = mono_image_open_from_data(buffer, size, 1, &status);

		assembly = mono_assembly_load_from(image, (std::string("assembly") + std::to_string(uuid)).data(), &status);

		delete buffer;

		if (!assembly || ! image)
		{
			//Somehow the .dll could not be found.
			return false;
		}

		//Referencing callback methods
		MonoMethodDesc* desc = mono_method_desc_new ((scriptName + ":Awake()").data(), false);
		awakeMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new ((scriptName + ":Start()").data(), false);
		startMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new ((scriptName + ":PreUpdate()").data(), false);
		preUpdateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new ((scriptName + ":Update()").data(), false);
		updateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new ((scriptName + ":PostUpdate()").data(), false);
		postUpdateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);
	}

	return ret;
}

std::string ResourceScript::pathToWindowsNotation(const std::string& path) const
{
	std::string ret = path;
	while (ret.find("/") != std::string::npos)
	{
		ret = ret.replace(ret.find("/"), 1, "\\");
	}
	return ret;
}
