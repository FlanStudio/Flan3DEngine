#include "ComponentScript.h"
#include "ResourceScript.h"

#include "imgui/imgui_internal.h"
#include "imgui/imgui_stl.h"

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
		if (isActive() && gameObject->areParentsActives())
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
		if (isActive() && gameObject->areParentsActives())
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
		if (isActive() && gameObject->areParentsActives())
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
			uint32_t flags = mono_field_get_flags(field);
			if (flags & MONO_FIELD_ATTR_PUBLIC && !(flags & MONO_FIELD_ATTR_STATIC))
			{
				//This field is public and not static.
				//Show the field, check the type and adapt the gui to it.
				MonoType* type = mono_field_get_type(field);
				
				std::string typeName = mono_type_full_name(type);

				if (typeName == "bool")
				{
					bool varState; mono_field_get_value(classInstance, field, &varState);
					if(ImGui::Checkbox(mono_field_get_name(field), &varState))
					{
						mono_field_set_value(classInstance, field, &varState);
					}
				}
				else if (typeName == "single") //this is a float, idk
				{					
					float varState; mono_field_get_value(classInstance, field, &varState);
					if (ImGui::InputFloat(mono_field_get_name(field), &varState))
					{
						mono_field_set_value(classInstance, field, &varState);
					}
				}
				else if (typeName == "double")
				{
					double varState;
				}
				else if (typeName == "System.Decimal")
				{
					//We cant convert System.Decimal, since we do not have this decimal precision in any C++ type.
				}
				else if (typeName == "sbyte")
				{
					int8_t varState;
				}
				else if (typeName == "byte")
				{
					uint8_t varState;

				}			
				else if (typeName == "int16")
				{
					int16_t varState;
				}
				else if (typeName == "uint16")
				{
					uint16_t varState;
				}
				else if (typeName == "int")
				{
					int32_t varState;
				}
				else if (typeName == "uint")
				{
					uint32_t varState;
				}
				else if (typeName == "long")
				{
					int64_t varState;
				}
				else if (typeName == "ulong")
				{
					uint64_t varState;
				}
				
				else if (typeName == "char")
				{
					//Characters in C# are 2 bytes, while they are only 1 in C++. We may be losing some data here, since C# chars suppport Unicode and we only support ASCII.
					char varState;
					mono_field_get_value(classInstance, field, &varState);

				}
				else if (typeName == "string")
				{
					MonoString* varState;
					mono_field_get_value(classInstance, field, &varState);

					char* convertedString = mono_string_to_utf8(varState);

					std::string stringToModify = convertedString;
					if (ImGui::InputText(mono_field_get_name(field), &stringToModify))
					{
						MonoString* newString = mono_string_new(App->scripting->domain, stringToModify.data());
						mono_field_set_value(classInstance, field, newString);
					}

					mono_free(convertedString);
				}

				else if (typeName == "FlanEngine.GameObject")
				{
					ImVec2 drawingPos = ImGui::GetCursorScreenPos();
					/*drawingPos = { drawingPos.x - 10, drawingPos.y };
					ImGui::SetCursorScreenPos(drawingPos);*/

					uint buttonWidth = 0.65 * ImGui::GetWindowWidth();
					ImGui::ButtonEx(std::string("##" + std::string(mono_field_get_name(field))).data(), { (float)buttonWidth, 20 }, ImGuiButtonFlags_::ImGuiButtonFlags_Disabled);

					if (ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingGOs", ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
						if (payload)
						{
							GameObject* go = *(GameObject**)payload->Data;
							
							if (ImGui::IsMouseReleased(0))
							{
								MonoObject* monoObject = App->scripting->MonoObjectFrom(go);
								mono_field_set_value(classInstance, field, monoObject);
							}
						}
						ImGui::EndDragDropTarget();
					}

					if (ImGui::IsItemClicked(0))
					{
						ImDrawList* drawList = ImGui::GetWindowDrawList();
						drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 20 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive));
					}
					else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
					{
						ImDrawList* drawList = ImGui::GetWindowDrawList();
						drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 20 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonHovered));

						if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
						{
							mono_field_set_value(classInstance, field, NULL);
						}
					}

					ImGui::SetCursorScreenPos({ drawingPos.x+5, drawingPos.y+3});

					MonoObject* monoObject;
					mono_field_get_value(classInstance, field, &monoObject);

					std::string text;

					if (monoObject)
					{
						bool destroyed;
						mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), "destroyed"), &destroyed);

						if (!destroyed)
						{
							MonoString* goName;
							mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), "name"), &goName);

							char* nameCpp = mono_string_to_utf8(goName);

							text = nameCpp + std::string(" (GameObject)");

							mono_free(nameCpp);
						}
						else
						{
							mono_field_set_value(classInstance, field, NULL);
						}					
					}
					else
					{
						text = "NULL (GameObject)";
					}



					ImGui::Text(text.data());

					ImGui::SameLine();
					
					ImGui::SetCursorScreenPos({drawingPos.x + buttonWidth + 5, drawingPos.y+3});
					ImGui::Text(mono_field_get_name(field));

					drawingPos = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos({ drawingPos.x, drawingPos.y+4});
				}
			}

			field = mono_class_get_fields(mono_object_get_class(classInstance), (void**)&iterator);
		}
		ImGui::NewLine();
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
