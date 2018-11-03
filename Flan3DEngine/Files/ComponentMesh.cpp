#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"

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
		ImGui::Text("Mesh: %s", gameObject->name.data()); //TODO: SAVE THE REAL MESH NAME
		ImGui::Text("Vertices: %i", num_vertex);
		ImGui::Text("Triangles: %i",num_vertex / 3);
		
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::TextColored({ .8f, 0.0f, 0.0f, 1.0f }, "TODO: Move this into ComponentMaterial when we can have Resources vs Components");

		for (int i = 0; i < App->textures->textures.size(); ++i)
		{
			ImGui::Text("Texture %i", i); ImGui::NewLine();
			ImGui::SetCursorPosX(36);
			ImGui::Image((GLuint*)App->textures->textures[i]->id, { 50,50 }, { 0,1 }, { 1,0 });
			ImGui::TextWrapped("\tFile: %s", App->textures->textures[i]->name.data());
			ImGui::Text("\tsize: %dx%d", App->textures->textures[i]->width, App->textures->textures[i]->height);
		}
	}
}

void ComponentMesh::updateGameObjectAABB()
{
	gameObject->boundingBox.Enclose((float3*)vertex, num_vertex);
}

void ComponentMesh::Serialize(char*& cursor) const
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &gameObject->UUID, bytes);
	cursor += bytes;

	uint ranges[5] =
	{
		num_vertex,
		num_index,
		colors ? num_vertex : 0,
		normals ? num_vertex : 0,
		textureCoords ? num_vertex : 0
	};

	//uint size = sizeof(ranges);
	//size += num_vertex * 3 * sizeof(float);
	//size += num_index * sizeof(uint);
	//if (colors)
	//{
	//	size += num_vertex * 4 * sizeof(float); //for each vertex 4 floats (rgba)
	//}
	//if (normals)
	//{
	//	size += num_vertex * 3 * sizeof(float); //for each vertex 3 floats (xyz)
	//}
	//if (textureCoords)
	//{
	//	size += num_vertex * 2 * sizeof(float); //for each vertex 2 floats (xy)
	//}

	//char* buffer = new char[size];
	//char* cursor = buffer; //Point to the beginning of the buffer

	//write ranges
	bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes; //displace the writting point by bytes

	//write vertex
	bytes = num_vertex * 3 * sizeof(float);
	memcpy(cursor, vertex, bytes);
	cursor += bytes;

	//write index
	bytes = num_index * sizeof(uint);
	memcpy(cursor, index, bytes);
	cursor += bytes;

	//write colors
	if (colors)
	{
		bytes = num_vertex * sizeof(float) * 4;
		memcpy(cursor, colors, bytes);
		cursor += bytes;
	}

	//write normals
	if (normals)
	{
		bytes = num_vertex * sizeof(float) * 3;
		memcpy(cursor, normals, bytes);
		cursor += bytes;
	}

	//write textureCoords
	if (textureCoords)
	{
		bytes = num_vertex * sizeof(float) * 2;
		memcpy(cursor, textureCoords, bytes);
		cursor += bytes;
	}
	
	//App->fs->OpenWriteBuffer("Library/Meshes/" + std::to_string(UUID) + std::string(".flanMesh"), buffer, size);
	//delete buffer;
	/*ComponentMesh* othermesh = new ComponentMesh(nullptr);
	LoadMesh(othermesh, "Library/" + mesh->name + ".jeje");*/
}

void ComponentMesh::DeSerialize(char*& cursor, uint32_t& goUUID)
{
	uint bytes = sizeof(uint32_t);
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	uint ranges[5];

	bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	num_vertex = ranges[0];
	num_index = ranges[1];

	bytes = num_vertex * sizeof(float) * 3;
	vertex = new float[num_vertex * 3];
	memcpy(vertex, cursor, bytes);
	cursor += bytes;

	bytes = num_index * sizeof(uint);
	index = new uint[num_index];
	memcpy(index, cursor, bytes);
	cursor += bytes;

	//vertex index colors normals textures
	if (ranges[2] > 0)
	{
		bytes = num_vertex * sizeof(float) * 4;
		colors = new float[num_vertex * 4];
		memcpy(colors, cursor, bytes);
		cursor += bytes;
	}

	if (ranges[3] > 0)
	{
		bytes = num_vertex * sizeof(float) * 3;
		normals = new float[num_vertex * 3];
		memcpy(normals, cursor, bytes);
		cursor += bytes;
	}

	if (ranges[4] > 0)
	{
		bytes = num_vertex * sizeof(float) * 2;
		textureCoords = new float[num_vertex * 2];
		memcpy(textureCoords, cursor, bytes);
		cursor += bytes;
	}

	genBuffers();
}

uint ComponentMesh::bytesToSerialize() const
{
	uint ranges[5] =
	{
		num_vertex,
		num_index,
		colors ? num_vertex : 0,
		normals ? num_vertex : 0,
		textureCoords ? num_vertex : 0
	};

	uint size = sizeof(ranges);
	size += num_vertex * 3 * sizeof(float);
	size += num_index * sizeof(uint);
	if (colors)
	{
		size += num_vertex * 4 * sizeof(float); //for each vertex 4 floats (rgba)
	}
	if (normals)
	{
		size += num_vertex * 3 * sizeof(float); //for each vertex 3 floats (xyz)
	}
	if (textureCoords)
	{
		size += num_vertex * 2 * sizeof(float); //for each vertex 2 floats (xy)
	}
	
	return size;
}
