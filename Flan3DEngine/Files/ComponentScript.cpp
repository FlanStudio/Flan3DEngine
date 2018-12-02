#include "ComponentScript.h"

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

		ImGui::NewLine();
	}

}
