#include "ResourceMesh.h"
#include "Glew/include/glew.h"

ResourceMesh::~ResourceMesh()
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

void ResourceMesh::genBuffers()
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

		genNormalLines(normalsLenght);
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

void ResourceMesh::destroyBuffers()
{
	glDeleteBuffers(1, &vertex_ID);
	glDeleteBuffers(1, &index_ID);
	glDeleteBuffers(1, &normals_ID);
	glDeleteBuffers(1, &normalLines_ID);
	glDeleteBuffers(1, &colors_ID);
	glDeleteBuffers(1, &textureCoords_ID);
	vertex_ID = index_ID = normals_ID = normalLines_ID = colors_ID = textureCoords_ID = 0;
}

void ResourceMesh::Draw()
{
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

	if (textureCoords_ID != 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoords_ID);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_ID);
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(1, 1, 1, 1);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ResourceMesh::UpdateNormalsLenght(uint lenght)
{
	normalsLenght = lenght;

	if (!normalLines)
		return;

	for (int i = 0; i < num_vertex; i++)
	{
		//origin coordinates
		normalLines[i * 6] = vertex[i * 3];									//x
		normalLines[i * 6 + 1] = vertex[i * 3 + 1];							//y
		normalLines[i * 6 + 2] = vertex[i * 3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3] = normals[i * 3] * lenght + normalLines[i * 6];				//x
		normalLines[i * 6 + 4] = normals[i * 3 + 1] * lenght + normalLines[i * 6 + 1];		//y
		normalLines[i * 6 + 5] = normals[i * 3 + 2] * lenght + normalLines[i * 6 + 2];		//z
	}

	glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3 * 2, normalLines, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::drawNormals()
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

void ResourceMesh::updateGameObjectAABB()
{
}

uint ResourceMesh::getBytes() const
{
	return sizeof(ResourceMesh);
}

uint ResourceMesh::bytesToSerialize() const
{
	uint nameLenght = meshName.length();

	uint ranges[5] =
	{
		num_vertex,
		num_index,
		colors ? num_vertex : 0,
		normals ? num_vertex : 0,
		textureCoords ? num_vertex : 0
	};

	uint size = sizeof(uint);
	size += nameLenght;
	size += sizeof(ranges);
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

void ResourceMesh::genNormalLines(uint lenght)
{
	normalLines = new float[num_vertex * 3 * 2];
	for (int i = 0; i < num_vertex; i++)
	{
		//origin coordinates
		normalLines[i * 6] = vertex[i * 3];									//x
		normalLines[i * 6 + 1] = vertex[i * 3 + 1];							//y
		normalLines[i * 6 + 2] = vertex[i * 3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3] = normals[i * 3] * lenght + normalLines[i * 6];				//x
		normalLines[i * 6 + 4] = normals[i * 3 + 1] * lenght + normalLines[i * 6 + 1];		//y
		normalLines[i * 6 + 5] = normals[i * 3 + 2] * lenght + normalLines[i * 6 + 2];		//z
	}
}

void ResourceMesh::Serialize(char*& cursor) const
{
	uint bytes = sizeof(uint);

	uint nameLenght = meshName.length();
	memcpy(cursor, &nameLenght, bytes);
	cursor += bytes;

	memcpy(cursor, meshName.c_str(), nameLenght);
	cursor += nameLenght;

	uint ranges[5] =
	{
		num_vertex,
		num_index,
		colors ? num_vertex : 0,
		normals ? num_vertex : 0,
		textureCoords ? num_vertex : 0
	};

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
}

void ResourceMesh::DeSerialize(char *& cursor, uint normalLenght)
{
	uint bytes = sizeof(uint);

	uint nameLenght;
	memcpy(&nameLenght, cursor, bytes);
	cursor += bytes;

	memcpy((void*)meshName.c_str(), cursor, nameLenght);
	cursor += nameLenght;

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
