#include "ResourceMesh.h"

ResourceMesh::~ResourceMesh()
{
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