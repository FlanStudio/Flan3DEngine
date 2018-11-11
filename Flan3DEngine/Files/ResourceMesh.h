#ifndef __RESOURCEMESH_H__
#define __RESOURCEMESH_H__

#include "Resource.h"

class ResourceMesh : public Resource
{
public:

	ResourceMesh() : Resource(ResourceType::MESH) {}
	virtual ~ResourceMesh();

public:
	void genBuffers();
	void destroyBuffers();
	void Draw();
	void UpdateNormalsLenght(uint lenght);
	void drawNormals();

	void updateGameObjectAABB();

	void Serialize(char*& cursor) const;
	void DeSerialize(char*& cursor, uint normalLenght);

	uint getBytes() const;

	uint bytesToSerialize()const;

	void genNormalLines(uint lenght);

public:

	uint normalsLenght = 0.5f;

	std::string meshName = "default";

	uint index_ID = 0;			// index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint vertex_ID = 0;		// unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint normals_ID = 0;
	float* normals = nullptr;

	uint normalLines_ID = 0;
	float* normalLines = nullptr;

	uint colors_ID = 0;
	float* colors = nullptr;

	uint textureCoords_ID = 0;
	float* textureCoords = nullptr;
};
#endif