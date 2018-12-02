#include "ScriptingModule.h"
#include "ComponentScript.h"

update_status ScriptingModule::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ScriptingModule::Update()
{
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

ComponentScript* ScriptingModule::CreateScript(std::string csPath)
{
	ComponentScript* script = new ComponentScript(csPath);
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
