#pragma once

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

class MeshComponent : public Component
{
public:

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

	MeshComponent(GameObject* parent, bool active = true) : Component(ComponentType::MESH, parent, active){}
	~MeshComponent();
	void genBuffers();
	void destroyBuffers();
	void Draw();
	void genNormalLines();
	void UpdateNormalsLenght();
	void drawNormals();
};