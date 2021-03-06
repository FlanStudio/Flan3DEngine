#include "ComponentMaterial.h"
#include "ResourceTexture.h"
#include "imgui/imgui_internal.h"
#include "GameObject.h"

ComponentMaterial::~ComponentMaterial()
{
	if (texture)
		texture->deReferenced();
}

void ComponentMaterial::Serialize(char *& cursor) const
{
	uint bytes = sizeof(UID);
	memcpy(cursor, &UUID, bytes);
	cursor += bytes;

	memcpy(cursor, &gameObject->uuid, bytes);
	cursor += bytes;

	UID textureUID = texture ? texture->getUUID() : 0;
	memcpy(cursor, &textureUID, bytes);
	cursor += bytes;

	bytes = sizeof(ImVec4);
	memcpy(cursor, &colorTint.x, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &active, bytes);
	cursor += bytes;
}

void ComponentMaterial::DeSerialize(char*& cursor, uint32_t& goUUID)
{
	uint bytes = sizeof(UID);

	memcpy(&UUID, cursor, bytes);
	cursor += bytes;
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	UID textureUID;
	memcpy(&textureUID, cursor, bytes);
	cursor += bytes;

	texture = (ResourceTexture*)App->resources->Get(textureUID);

	if (texture)
		texture->Referenced();

	bytes = sizeof(ImVec4);
	memcpy(&colorTint.x, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(&active, cursor, bytes);
	cursor += bytes;
}

void ComponentMaterial::OnInspector()
{
	ImGui::Checkbox(("###ACTIVE_MATERIAL" + std::to_string(UUID)).data(), &this->active); ImGui::SameLine();
	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader("##Material"); ImGui::SameLine();

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("RightClickMaterialMenu");
	}

	ImGui::SetNextWindowSize({ 150, 45 });

	ImGuiWindowFlags wflags = 0;
	wflags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopup("RightClickMaterialMenu", wflags))
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
		ImGui::Text("Material");
		ImGui::EndTooltip();
		ComponentTransform* thisOne = (ComponentTransform*)this;
		ImGui::SetDragDropPayload("DraggingComponents", &thisOne, sizeof(ComponentMaterial));
		ImGui::EndDragDropSource();
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text("Material:");
	if (opened)
	{
		ImGui::NewLine();

		ImGui::Text("Texture:  "); ImGui::SameLine();

		ImVec2 drawingPos = ImGui::GetCursorScreenPos();
		drawingPos = { drawingPos.x - 10, drawingPos.y };
		ImGui::SetCursorScreenPos(drawingPos);
	
		uint buttonWidth = 2 * ImGui::GetWindowWidth() / 3;
		ImGui::ButtonEx("##TextureReceiver", { (float)buttonWidth, 15 }, ImGuiButtonFlags_::ImGuiButtonFlags_Disabled);
			
		if (ImGui::IsMouseClicked(0) && !ImGui::IsItemClicked(0))
		{
			textureClicked = false;
		}
		else if (ImGui::IsItemClicked(0))
		{
			textureClicked = true;
		}
		else if (!textureClicked && ImGui::IsItemHovered())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonHovered));
		}

		if(textureClicked)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive));
			if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
			{
				if(texture)
					texture->deReferenced();
				texture = nullptr;
				textureClicked = false;
			}
		}	

		//Dropping textures
		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags dropFlags = 0;
			dropFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;

			const ImGuiPayload* texturePayload = ImGui::AcceptDragDropPayload("DraggingResources", dropFlags);
			if (texturePayload)
			{
				//Check if the resource is a texture
				Resource* resource = *(Resource**)texturePayload->Data;
				if (resource->getType() == Resource::ResourceType::TEXTURE)
				{
					//Check for the mouse release and bind the resource here
					if (ImGui::IsMouseReleased(0))
					{
						if (texture)
							texture->deReferenced();
						texture = (ResourceTexture*)resource;
						texture->Referenced();
					}
				}
			}


			ImGui::EndDragDropTarget();
		}
		
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Drop here a Texture.\nSelect it and press Backspace to erase the reference.");
			ImGui::EndTooltip();
		}
		ImGui::SetCursorScreenPos({ drawingPos.x + 7, drawingPos.y });

		//Calculate the text fitting the button rect
		std::string originalText = texture ? texture->getFile() : "No texture assigned";
		std::string clampedText;
		
		ImVec2 textSize = ImGui::CalcTextSize(originalText.data());
		
		if (textSize.x > buttonWidth-5)
		{
			uint maxTextLenght = originalText.length() * (buttonWidth - 5) / textSize.x;
			clampedText = originalText.substr(0, maxTextLenght - 5);
			clampedText.append("(...)");
		}
		else
			clampedText = originalText;

		ImGui::Text(clampedText.data());

		//Tinting textures		
		drawingPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos({ drawingPos.x, drawingPos.y+5});

		ImGui::Text("Tint: "); ImGui::SameLine();

		drawingPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos({ drawingPos.x, drawingPos.y - 5 });

		ImGuiColorEditFlags colorFlags = 0;
		colorFlags |= ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs;
		colorFlags |= ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar;
		
		ImGui::ColorEdit4("##TextureTint", &colorTint.x, colorFlags);

		ImGui::NewLine();

		if (texture)
		{
			ImGui::Image((GLuint*)texture->id, { 50, 50 });
			ImGui::Text("Size: %dx%d", texture->width, texture->height);
		}

		ImGui::NewLine();
	}
}
