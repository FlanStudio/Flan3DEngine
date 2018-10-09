#pragma once

#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class RayPrim : public Primitive
{
public:
	RayPrim() : Primitive() { }
	RayPrim(float3 pos) : Primitive(pos) { }
	~RayPrim();

public:
	void Init();
	void pRender();

public:
	float points[6] = { -5.0f,0.0f,0.0f, 5.0f,0.0f,0.0f };//begin and end

private:
	uint vertexID = 0;
	std::vector<float> vertex;
};
