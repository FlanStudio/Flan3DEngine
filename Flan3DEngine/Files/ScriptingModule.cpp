#include "ScriptingModule.h"
#include "ComponentScript.h"

#include "pugui/pugixml.hpp"

bool ScriptingModule::Start()
{
	CreateScriptingProject();
	return true;
}

update_status ScriptingModule::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ScriptingModule::Update()
{
	//static float timeLapsed = 0.0f;
	//timeLapsed += App->time->dt;

	//if (timeLapsed >= .5)
	//{
	//	timeLapsed == 0;
	//	//IncludecsFiles();
	//}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		CreateScriptingProject();
		IncludecsFiles();
	}

	return UPDATE_CONTINUE;
}

update_status ScriptingModule::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ScriptingModule::CleanUp()
{
	for (int i = 0; i < scripts.size(); ++i)
	{
		delete scripts[i];
	}
	scripts.clear();
	return true;
}

void ScriptingModule::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::PLAY:
		{
			//Call the Script Awake if both script and gameObject are active. If not, we must call it during runtime if it becomes active.
			for (int i = 0; i < scripts.size(); ++i)
			{
				GameObject* gameObject = scripts[i]->gameObject;
				if (!gameObject)
					continue;

			
			}

			//Call the Script Start if both script and gameObject are active.
			for (int i = 0; i < scripts.size(); ++i)
			{
				GameObject* gameObject = scripts[i]->gameObject;
				if (!gameObject)
					continue;



			}
		}
	}
}

ComponentScript* ScriptingModule::CreateScript(std::string scriptName)
{
	while (scriptName.find(" ") != std::string::npos)
	{
		scriptName = scriptName.replace(scriptName.find(" "), 1, "");
	}

	ComponentScript* script = new ComponentScript(scriptName);
	char* buffer;
	int size;

	App->fs->OpenRead("Internal/SampleScript/SampleScript.cs", &buffer, size);

	std::string scriptStream = buffer;
	scriptStream.resize(size);

	while (scriptStream.find("SampleScript") != std::string::npos)
	{
		scriptStream = scriptStream.replace(scriptStream.find("SampleScript"), 12, scriptName);
	}

	App->fs->OpenWriteBuffer("Assets/Scripts/" + scriptName + ".cs", (char*)scriptStream.c_str(), scriptStream.size());

	delete buffer;

	script->csPath = "Assets/Scripts/" + scriptName + ".cs";

	scripts.push_back(script);
	return script;
}

bool ScriptingModule::DestroyScript(ComponentScript* script)
{
	for (int i = 0; i < scripts.size(); ++i)
	{
		if (scripts[i] == script)
		{
			delete script;
			scripts.erase(scripts.begin() + i);
			return true;
		}
	}

	return false;
}

bool ScriptingModule::alreadyCreated(std::string scriptName)
{
	clearSpaces(scriptName);

	for (int i = 0; i < scripts.size(); ++i)
	{
		if (scriptName == scripts[i]->scriptName)
			return true;
	}

	return false;
}

void ScriptingModule::CreateScriptingProject()
{
	if (App->fs->Exists("ScriptingProject.sln"))
		return;

	App->fs->CopyDirectoryAndContentsInto("Internal/ScriptingProject", "", false);
}

void ScriptingModule::ExecuteScriptingProject()
{
	//We need the path to the Visual Studio .exe and after that ask the program to start opening the right solution. No idea how to do that for now.
#if 0
	CreateScriptingProject();

	IncludecsFiles();

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	bool ret = CreateProcess("C:\\Users\\Jony635\\Desktop\\Proyectos 3o\\GitHub\\Flan3DEngine\\Flan3DEngine\\Game\\ScriptingProject.sln", "C:\\Users\\Jony635\\Desktop\\Proyectos 3o\\GitHub\\Flan3DEngine\\Flan3DEngine\\Game\\ScriptingProject.sln", 0, 0, false, 0, 0, 0, &si, &pi) != 0;

	if (!ret)
	{
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		Debug.LogError("Could not open \"ScriptingProject.sln\". Error: %s", message.data());

		//Free the buffer.
		LocalFree(messageBuffer);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
#endif
}

void ScriptingModule::IncludecsFiles()
{
	Directory scripts = App->fs->getDirFiles("Assets/Scripts");

	//Modify the project settings file, stored in a xml

	char* buffer;
	int size;
	if (!App->fs->OpenRead("Assembly-CSharp.csproj", &buffer, size))
		return;

	pugi::xml_document configFile;
	configFile.load_buffer(buffer, size);
	pugi::xml_node projectNode = configFile.child("Project");
	pugi::xml_node itemGroup = projectNode.child("ItemGroup").next_sibling();
	
	for (pugi::xml_node compile = itemGroup.child("Compile"); compile != nullptr;)
	{
		pugi::xml_node next = compile.next_sibling();

		//Do not delete the internal files
		if (std::string(compile.attribute("Include").as_string()).find("internal") != std::string::npos)
		{
			compile = next;
			continue;
		}
			
		itemGroup.remove_child(compile);
		compile = next;
	}

	for (int i = 0; i < scripts.files.size(); ++i)
	{	
		itemGroup.append_child("Compile").append_attribute("Include").set_value(std::string("Assets\\Scripts\\" + scripts.files[i].name).data());
	}

	std::ostringstream stream;
	configFile.save(stream, "\r\n");
	App->fs->OpenWriteBuffer("Assembly-CSharp.csproj", (char*)stream.str().data(), stream.str().size());	
}

std::string ScriptingModule::clearSpaces(std::string& scriptName)
{
	while (scriptName.find(" ") != std::string::npos)
	{
		scriptName = scriptName.replace(scriptName.find(" "), 1, "");
	}
	return scriptName;
}
