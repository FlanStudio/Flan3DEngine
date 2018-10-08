#pragma once

#include "Primitive.h"
#include "Globals.h"
#include <vector>

class GridPrim : public Primitive
{
public:
	GridPrim() : Primitive() { }
	GridPrim(float3 pos) : Primitive(pos) { }
	~GridPrim();

public:
	void Init();
	void pRender();

public:
	float length = 100.0f;
	uint cuts = 100;

private:
	uint vertexID = 0;
	std::vector<float> vertex;
};
