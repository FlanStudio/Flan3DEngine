#include "ScriptingModule.h"
#include "ComponentScript.h"

update_status ScriptingModule::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ScriptingModule::Update()
{
	//Temporal for testing
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		CreateScriptingProject();
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

std::string ScriptingModule::clearSpaces(std::string& scriptName)
{
	while (scriptName.find(" ") != std::string::npos)
	{
		scriptName = scriptName.replace(scriptName.find(" "), 1, "");
	}
	return scriptName;
}
