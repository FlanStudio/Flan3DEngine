#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"

ComponentMesh::~ComponentMesh()
{
	destroyBuffers();
	delete[] normals;
	delete[] vertex;
	delete[] index;
	delete[] normalLines;
	delete[] colors;
	delete[] textureCoords;
	normals = vertex = normalLines = colors = textureCoords = nullptr;
	index = nullptr;
}

void ComponentMesh::genBuffers()
{
	glGenBuffers(1, &vertex_ID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &index_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (normals)
	{
		glGenBuffers(1, &normals_ID);
		glBindBuffer(GL_ARRAY_BUFFER, normals_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		genNormalLines();
		glGenBuffers(1, &normalLines_ID);
		glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3 * 2, normalLines, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (colors)
	{
		glGenBuffers(1, &colors_ID);
		glBindBuffer(GL_ARRAY_BUFFER, colors_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 4, colors, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (textureCoords)
	{
		glGenBuffers(1, &textureCoords_ID);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoords_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 2, textureCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void ComponentMesh::destroyBuffers()
{
	glDeleteBuffers(1, &vertex_ID);
	glDeleteBuffers(1, &index_ID);
	glDeleteBuffers(1, &normals_ID);
	glDeleteBuffers(1, &normalLines_ID);
	glDeleteBuffers(1, &colors_ID);
	glDeleteBuffers(1, &textureCoords_ID);
	vertex_ID = index_ID = normals_ID = normalLines_ID = colors_ID = textureCoords_ID = 0;
}

void ComponentMesh::Draw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	float4x4 myMatrix = gameObject->transform->getMatrix();

	glMultMatrixf(myMatrix.ptr());

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normals_ID);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colors_ID);
	glColorPointer(4, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, App->textures->textures.empty() ? 0 : App->textures->textures[0]->id);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoords_ID);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_ID);
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void ComponentMesh::genNormalLines()
{
	normalLines = new float[num_vertex * 3 * 2];
	for (int i = 0; i < num_vertex; i++)
	{
		//origin coordinates
		normalLines[i * 6] = vertex[i * 3];									//x
		normalLines[i * 6 + 1] = vertex[i * 3 + 1];							//y
		normalLines[i * 6 + 2] = vertex[i * 3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3] = normals[i * 3] * App->renderer3D->normalsLenght + normalLines[i * 6];				//x
		normalLines[i * 6 + 4] = normals[i * 3 + 1] * App->renderer3D->normalsLenght + normalLines[i * 6 + 1];		//y
		normalLines[i * 6 + 5] = normals[i * 3 + 2] * App->renderer3D->normalsLenght + normalLines[i * 6 + 2];		//z
	}
}

void ComponentMesh::UpdateNormalsLenght()
{
	if (!normalLines)
		return;

	for (int i = 0; i < num_vertex; i++)
	{
		//origin coordinates
		normalLines[i * 6] = vertex[i * 3];									//x
		normalLines[i * 6 + 1] = vertex[i * 3 + 1];							//y
		normalLines[i * 6 + 2] = vertex[i * 3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3] = normals[i * 3] * App->renderer3D->normalsLenght + normalLines[i * 6];				//x
		normalLines[i * 6 + 4] = normals[i * 3 + 1] * App->renderer3D->normalsLenght + normalLines[i * 6 + 1];		//y
		normalLines[i * 6 + 5] = normals[i * 3 + 2] * App->renderer3D->normalsLenght + normalLines[i * 6 + 2];		//z
	}

	glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3 * 2, normalLines, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::drawNormals()
{
	if (normalLines)
	{
		glColor3f(1, 0, 0);

		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, num_vertex * 2);

		glDisableClientState(GL_VERTEX_ARRAY);

		glColor3f(1, 1, 1);
	}
}

void ComponentMesh::OnInspector()
{
	float PosX = ImGui::GetCursorPosX();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, { .2,.2,.9,.5 });
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

	ImGui::PopStyleColor();
	ImGui::SetCursorPosX(PosX + 20);
	ImGui::TextColored({ 1,1,0,1 }, "Mesh:");
	if (opened)
	{
		ImGui::Text("Mesh: %s", gameObject->name.data()); //TODO: SAVE THE REAL MESH NAME
		ImGui::Text("Vertices: %i", num_vertex);
		ImGui::Text("Triangles: %i",num_vertex / 3);
	}
}