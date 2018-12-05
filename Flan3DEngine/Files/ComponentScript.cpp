#include "ComponentScript.h"

#include "imgui/imgui_internal.h"

#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#include <array>

void ComponentScript::Awake()
{
	if (initialized)
		return;

	//This will not be here
	if (!CompileCSFile())
	{
		initialized = false;
		return;
	}	
	initialized = true;

	if (awakeMethod)
	{		
		mono_runtime_invoke(awakeMethod, classInstance, NULL, NULL);
	}
}

void ComponentScript::Start()
{
	if (startMethod)
	{
		mono_runtime_invoke(startMethod, classInstance, NULL, NULL);
	}
}

void ComponentScript::PreUpdate()
{
	if (preUpdateMethod)
	{
		mono_runtime_invoke(preUpdateMethod, classInstance, NULL, NULL);
	}
}

void ComponentScript::Update()
{
	if (updateMethod)
	{
		mono_runtime_invoke(updateMethod, classInstance, NULL, NULL);
	}
}

void ComponentScript::PostUpdate()
{
	if (postUpdateMethod)
	{
		mono_runtime_invoke(postUpdateMethod, classInstance, NULL, NULL);
	}
}

void ComponentScript::OnInspector()
{
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
			ImGui::Text("\"%s\"\n\nThe .cs file attached to this script component.\nDo not move the script for now!", csPath.data());
			ImGui::EndTooltip();
		}		

		ImGui::SetCursorScreenPos({ drawingPos.x + 7, drawingPos.y });

		//Calculate the text fitting the button rect
		std::string originalText = csPath;
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
	}

}

bool ComponentScript::CompileCSFile()
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
	std::string path = std::string("\"" + std::string(App->fs->getAppPath())) + "Assets\\Scripts\\" + scriptName + ".cs\" ";

	std::string error;

	if (!exec(std::string(goRoot + "&" + goMonoBin + "&" + compileCommand + path + App->scripting->getReferencePath()).data(), error))
	{
		ret = false;
		if (!error.empty())
			Debug.LogError("Error compiling the script %s. Error: %s", (scriptName + ".cs").data(), error.data());
		else
			Debug.LogError("Error compiling the script %s.");
	}

	if (ret)
	{
		//TODO: Move the dll to the Library folder.

		//Reference the methods
		assembly = mono_domain_assembly_open(App->scripting->domain, std::string("Assets\\Scripts\\" + scriptName + ".dll").data());
		if (!assembly)
		{
			//Somehow the .dll could not be found.
			return false;
		}

		image = mono_assembly_get_image(assembly);
		if (!image)
			return ret;

		MonoClass* klass = mono_class_from_name(image, "", scriptName.data());
		classInstance = mono_object_new(App->scripting->domain, klass);
		mono_runtime_object_init(classInstance);

		MonoMethodDesc* desc = mono_method_desc_new(std::string(scriptName + ":Awake()").data(), false);
		awakeMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new(std::string(scriptName + ":Start()").data(), false);
		startMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new(std::string(scriptName + ":PreUpdate()").data(), false);
		preUpdateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new(std::string(scriptName + ":Update()").data(), false);
		updateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);

		desc = mono_method_desc_new(std::string(scriptName + ":PostUpdate()").data(), false);
		postUpdateMethod = mono_method_desc_search_in_image(desc, image);
		mono_method_desc_free(desc);
	}

	return ret;
}
