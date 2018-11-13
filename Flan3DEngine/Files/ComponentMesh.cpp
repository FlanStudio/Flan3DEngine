#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"
#include "ResourceTexture.h"
#include "ComponentMaterial.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"

ComponentMesh::~ComponentMesh()
{
	
}

void ComponentMesh::genBuffers()
{
	
}

void ComponentMesh::destroyBuffers()
{
	
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
	if (material)
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

void ComponentMesh::genNormalLines()
{
	
}

void ComponentMesh::drawNormals()
{
	if (mesh)
		mesh->drawNormals();
}

void ComponentMesh::OnInspector()
{
	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader("##Mesh"); ImGui::SameLine();

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
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text("Mesh:");
	if (opened)
	{
		//Change that to the new Resources System

		/*ImGui::Text("Mesh: %s", gameObject->name.data()); //TODO: SAVE THE REAL MESH NAME
		//ImGui::Text("Vertices: %i", num_vertex);
		//ImGui::Text("Triangles: %i",num_vertex / 3);
		//
		//ImGui::NewLine();
		//ImGui::Separator();
		//ImGui::NewLine();
		//}*/
	}
}

void ComponentMesh::updateGameObjectAABB()
{
	if(mesh)
		gameObject->boundingBox.Enclose((float3*)mesh->vertex, mesh->num_vertex);
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
}
