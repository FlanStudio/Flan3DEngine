#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"
#include "ResourceTexture.h"
#include "ComponentMaterial.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"
#include "imgui/imgui_internal.h"

ComponentMesh::~ComponentMesh()
{
	if (mesh)
		mesh->deReferenced();
}

void ComponentMesh::Draw()
{
	if (!gameObject || !mesh)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	float4x4 myMatrix = gameObject->transform->getGlobalMatrix();

	glMultMatrixf(myMatrix.ptr());

	ComponentMaterial* material = (ComponentMaterial*)gameObject->getComponentByType(ComponentType::MATERIAL);
	if (material && material->isActive())
	{
		if (material->texture && mesh->textureCoords_ID == 0)
		{
			Debug.LogWarning("WARNING: GameObject %s asociated Mesh has no UVs set! This may cause weird behavior", gameObject->name.data());
		}
			
		glBindTexture(GL_TEXTURE_2D, material->texture ? material->texture->id : 0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4f(material->colorTint.x, material->colorTint.y, material->colorTint.z, material->colorTint.w);
	}

	mesh->Draw();

	glPopMatrix();
}

void ComponentMesh::UpdateNormalsLenght(uint newLenght)
{
	if (mesh && mesh->normalsLenght != newLenght)
		mesh->UpdateNormalsLenght(newLenght);
}

void ComponentMesh::drawNormals()
{
	if (mesh)
		mesh->drawNormals();
}

void ComponentMesh::OnInspector()
{
	ImGui::Checkbox(("###ACTIVE_MESH" + std::to_string(UUID)).data(), &this->active); ImGui::SameLine();

	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader("##Mesh"); ImGui::SameLine();

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("RightClickMeshMenu");
	}

	ImGui::SetNextWindowSize({ 150, 45 });

	ImGuiWindowFlags wflags = 0;
	wflags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopup("RightClickMeshMenu", wflags))
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
		ImGui::Text("Mesh");
		ImGui::EndTooltip();
		ComponentMesh* thisOne = (ComponentMesh*)this;
		ImGui::SetDragDropPayload("DraggingComponents", &thisOne, sizeof(ComponentMesh));
		ImGui::EndDragDropSource();

		gameObject->transformAABB();
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text("Mesh:");
	if (opened)
	{
		ImGui::NewLine();

		ImGui::Text("Mesh:  "); ImGui::SameLine();

		ImVec2 drawingPos = ImGui::GetCursorScreenPos();
		drawingPos = { drawingPos.x - 10, drawingPos.y };
		ImGui::SetCursorScreenPos(drawingPos);

		uint buttonWidth = 2 * ImGui::GetWindowWidth() / 3;
		ImGui::ButtonEx("##MeshReceiver", { (float)buttonWidth, 15 }, ImGuiButtonFlags_::ImGuiButtonFlags_Disabled);

		if (ImGui::IsMouseClicked(0) && !ImGui::IsItemClicked(0))
		{
			meshClicked = false;
		}
		else if (ImGui::IsItemClicked(0))
		{
			meshClicked = true;
		}
		else if (!meshClicked && ImGui::IsItemHovered())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonHovered));
		}

		if (meshClicked)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(drawingPos, { drawingPos.x + buttonWidth, drawingPos.y + 15 }, ImGui::GetColorU32(ImGuiCol_::ImGuiCol_ButtonActive));
			if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
			{
				//TODO: Notify the resource that you don't need him anymore
				mesh = nullptr;
				meshClicked = false;
				gameObject->initAABB();
				gameObject->transformAABB();
			}
		}

		//Dropping textures
		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags dropFlags = 0;
			dropFlags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;

			const ImGuiPayload* meshPayload = ImGui::AcceptDragDropPayload("DraggingResources", dropFlags);
			if (meshPayload)
			{
				//Check if the resource is a texture
				Resource* resource = *(Resource**)meshPayload->Data;
				if (resource->getType() == Resource::ResourceType::MESH)
				{
					//Check for the mouse release and bind the resource here
					if (ImGui::IsMouseReleased(0))
					{
						mesh = (ResourceMesh*)resource;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Drop here a Mesh from a FBX.\nSelect it and press Backspace to erase the reference.");
			ImGui::EndTooltip();
		}
		ImGui::SetCursorScreenPos({ drawingPos.x + 7, drawingPos.y });

		//Calculate the text fitting the button rect
		std::string originalText = mesh ? mesh->meshName : "No mesh assigned";
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

		//Current mesh data
		if (mesh)
		{
			ImGui::TextWrapped("\tNum Vertex: %d", mesh->num_vertex);
			ImGui::TextWrapped("\tHas Normals: %s", mesh->normals ? "true" : "false");
			ImGui::TextWrapped("\tHas UVs set: %s", mesh->textureCoords ? "true" : "false");
		}
		ImGui::NewLine();
	}
}

void ComponentMesh::updateGameObjectAABB()
{
	if(mesh)
		gameObject->initialAABB.Enclose((float3*)mesh->vertex, mesh->num_vertex);
}

void ComponentMesh::Serialize(char*& cursor) const
{
	uint bytes = sizeof(UID);

	memcpy(cursor, &UUID, bytes);
	cursor += bytes;
	memcpy(cursor, &gameObject->uuid, bytes);
	cursor += bytes;

	UID meshUID = mesh ? mesh->getUUID() : 0;
	memcpy(cursor, &meshUID, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &active, bytes);
	cursor += bytes;
}

void ComponentMesh::DeSerialize(char*& cursor, uint32_t& goUUID)
{
	uint bytes = sizeof(UID);

	memcpy(&UUID, cursor, bytes);
	cursor += bytes;
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	UID meshUID;
	memcpy(&meshUID, cursor, bytes);
	cursor += bytes;

	mesh = (ResourceMesh*)App->resources->Get(meshUID);
	if (mesh)
		mesh->Referenced();

	bytes = sizeof(bool);
	memcpy(&active,cursor, bytes);
	cursor += bytes;
}