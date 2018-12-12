#include "ComponentScript.h"
#include "ResourceScript.h"

#include "imgui/imgui_internal.h"

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#include <mono/metadata/attrdefs.h>


void ComponentScript::Awake()
{
	if (scriptRes && scriptRes->awakeMethod)
	{
		MonoObject* exc = nullptr;
		mono_runtime_invoke(scriptRes->awakeMethod, classInstance, NULL, &exc);
		if (exc)
		{
			//TODO: PAUSE THE ENGINE HERE
			MonoString* exceptionMessage = mono_object_to_string(exc, NULL);
			char* toLogMessage = mono_string_to_utf8(exceptionMessage);
			Debug.LogError(toLogMessage);
			mono_free(toLogMessage);
		}
	}
}

void ComponentScript::Start()
{
	if (scriptRes && scriptRes->startMethod)
	{
		MonoObject* exc = nullptr;
		mono_runtime_invoke(scriptRes->startMethod, classInstance, NULL, &exc);
		if (exc)
		{
			//TODO: PAUSE THE ENGINE HERE
			MonoString* exceptionMessage = mono_object_to_string(exc, NULL);
			char* toLogMessage = mono_string_to_utf8(exceptionMessage);
			Debug.LogError(toLogMessage);
			mono_free(toLogMessage);
		}
	}
}

void ComponentScript::PreUpdate()
{
	if (scriptRes && scriptRes->preUpdateMethod)
	{
		MonoObject* exc = nullptr;
		if (isActive())
		{
			mono_runtime_invoke(scriptRes->preUpdateMethod, classInstance, NULL, &exc);
			if (exc)
			{
				//TODO: PAUSE THE ENGINE HERE
				MonoString* exceptionMessage = mono_object_to_string(exc, NULL);
				char* toLogMessage = mono_string_to_utf8(exceptionMessage);
				Debug.LogError(toLogMessage);
				mono_free(toLogMessage);
			}
		}
	}
}

void ComponentScript::Update()
{
	if (scriptRes && scriptRes->updateMethod)
	{
		MonoObject* exc = nullptr;
		if (isActive())
		{
			mono_runtime_invoke(scriptRes->updateMethod, classInstance, NULL, &exc);
			if (exc)
			{
				//TODO: PAUSE THE ENGINE HERE
				MonoString* exceptionMessage = mono_object_to_string(exc, NULL);
				char* toLogMessage = mono_string_to_utf8(exceptionMessage);
				Debug.LogError(toLogMessage);
				mono_free(toLogMessage);
			}
		}
	}
}

void ComponentScript::PostUpdate()
{
	if (scriptRes && scriptRes->postUpdateMethod)
	{
		MonoObject* exc = nullptr;
		if (isActive())
		{
			mono_runtime_invoke(scriptRes->postUpdateMethod, classInstance, NULL, &exc);
			if (exc)
			{
				//TODO: PAUSE THE ENGINE HERE
				MonoString* exceptionMessage = mono_object_to_string(exc, NULL);
				char* toLogMessage = mono_string_to_utf8(exceptionMessage);
				Debug.LogError(toLogMessage);
				mono_free(toLogMessage);
			}
		}
	}
}

void ComponentScript::OnInspector()
{
	ImGui::Checkbox(("###ACTIVE_SCRIPT" + std::to_string(UUID)).data(), &this->active); ImGui::SameLine();

	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader(std::string("##Script" + std::to_string(UUID)).data()); ImGui::SameLine();

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup(std::string("##Script" + std::to_string(UUID)).data());
	}

	ImGui::SetNextWindowSize({ 150, 45 });

	ImGuiWindowFlags wflags = 0;
	wflags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopup(std::string("##Script" + std::to_string(UUID)).data(), wflags))
	{
		if (ImGui::MenuItem("Delete Component"))
		{
			Event event;
			event.type = EventType::COMPONENT_DESTROYED;
			event.compEvent.component = this;
			App->SendEvent(event);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGuiDragDropFlags flags = 0;
	flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
	if (ImGui::BeginDragDropSource(flags))
	{
		ImGui::BeginTooltip();
		ImGui::Text(std::string(scriptName + " (Script)").data());
		ImGui::EndTooltip();
		ComponentScript* thisOne = (ComponentScript*)this;
		ImGui::SetDragDropPayload("DraggingComponents", &thisOne, sizeof(ComponentScript));
		ImGui::EndDragDropSource();
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text(std::string(scriptName + " (Script)").data());
	if (opened)
	{
		ImGui::NewLine();
		ImGui::Text(".cs File:  "); ImGui::SameLine();

		ImVec2 drawingPos = ImGui::GetCursorScreenPos();
		drawingPos = { drawingPos.x - 10, drawingPos.y };
		ImGui::SetCursorScreenPos(drawingPos);

		uint buttonWidth = 2 * ImGui::GetWindowWidth() / 3;
		ImGui::ButtonEx("##csFile", { (float)buttonWidth, 15 }, ImGuiButtonFlags_::ImGuiButtonFlags_Disabled);

		if (ImGui::IsItemHovered())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonHovered));
		}

		if (ImGui::IsItemClicked(0))
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive));			
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("\"%s\"\n\nThe .cs file attached to this script component.\nDo not move the script for now!", scriptRes->getFile().data());
			ImGui::EndTooltip();
		}		

		ImGui::SetCursorScreenPos({ drawingPos.x + 7, drawingPos.y });

		//Calculate the text fitting the button rect
		std::string originalText = scriptRes ? scriptRes->getFile().data() : "";
		std::string clampedText;

		ImVec2 textSize = ImGui::CalcTextSize(originalText.data());

		if (textSize.x > buttonWidth - 5)
		{
			uint maxTextLenght = originalText.length() * (buttonWidth - 5) / textSize.x;
			clampedText = originalText.substr(0, maxTextLenght - 5);
			clampedText.append("(...)");
		}
		else
			clampedText = originalText;

		ImGui::Text(clampedText.data());
		ImGui::NewLine();

		//Script fields
		Debug.Clear();
		void* iterator = 0;
		MonoClassField* field = mono_class_get_fields(mono_object_get_class(classInstance), &iterator);

		while (field != nullptr)
		{
			MonoType* type = mono_field_get_type(field);
						
			if (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC)
			{
				Debug.Log("This field is public: %s", mono_field_get_name(field));

				//Show the field, check the type and adapt the gui to it.



			}

			field = mono_class_get_fields(mono_object_get_class(classInstance), (void**)&iterator);
		}
	}
}

void ComponentScript::Serialize(char*& cursor) const
{
	uint bytes = sizeof(UID);
	memcpy(cursor, &gameObject->uuid, bytes);
	cursor += bytes;

	memcpy(cursor, &UUID, bytes);
	cursor += bytes;

	UID resUID = scriptRes ? scriptRes->getUUID() : 0;
	memcpy(cursor, &resUID, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &active, bytes);
	cursor += bytes;
}

void ComponentScript::deSerialize(char*& cursor, uint32_t& goUUID)
{
	uint bytes = sizeof(UID);
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	memcpy(&UUID, cursor, bytes);
	cursor += bytes;

	UID resUID;
	memcpy(&resUID, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(&active, cursor, bytes);
	cursor += bytes;

	//Reference the ScriptResource
	scriptRes = (ResourceScript*)App->resources->Get(resUID);

	if (scriptRes)
	{
		scriptName = scriptRes->scriptName;
	}		
	else
		Debug.LogError("A ComponentScript lost his ResourceScript reference!");
}

void ComponentScript::InstanceClass()
{
	if (!scriptRes || scriptRes->state != ResourceScript::ScriptState::COMPILED_FINE)
		return;

	if (handleID != 0)
	{
		mono_gchandle_free(handleID);
		handleID = 0;
	}

	MonoClass* klass = mono_class_from_name(scriptRes->image, "", scriptName.data());
	classInstance = mono_object_new(App->scripting->domain, klass);
	
	mono_runtime_object_init(classInstance);

	//Create the monoObject and store it in the map
	MonoClass* gameObjectClass = mono_class_from_name(App->scripting->internalImage, "FlanEngine", "GameObject");
	MonoObject* monoGO = mono_object_new(App->scripting->domain, gameObjectClass);

	MonoMethodDesc* desc = mono_method_desc_new("FlanEngine.GameObject:.ctor", true); 
	MonoMethod* constructor = mono_method_desc_search_in_class(desc, gameObjectClass); 
	mono_method_desc_free(desc);

	mono_runtime_invoke(constructor, monoGO, NULL, NULL);

	//SetUp this monoGO inside the class Instance
	MonoClassField* instanceMonoGo = mono_class_get_field_from_name(klass, "gameObject");
	mono_field_set_value(classInstance, instanceMonoGo, monoGO);

	//Create the handle storage to make sure the garbage collector doesn't delete the classInstance
	handleID = mono_gchandle_new(classInstance, true);

	uint32_t handleID_monoObject = mono_gchandle_new(monoGO, true);

	App->scripting->gameObjectsMap.push_back(std::pair<GameObject*, uint32_t>(gameObject, handleID_monoObject));
}
