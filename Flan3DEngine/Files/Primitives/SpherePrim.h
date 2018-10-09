#pragma once

#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class SpherePrim : public Primitive
{
public:
	SpherePrim() : Primitive() { }
	SpherePrim(float3 pos) : Primitive(pos) { }
	~SpherePrim();

public:
	void Init();
	void pRender();

public:
	float radius = 5.0f;
	uint cuts = 20;

private:
	uint vertexID = 0;
	std::vector<float> vertex;

	uint vertex2ID = 0;
	std::vector<float> vertex2;

	//std::vector<float> vertex;
	//std::vector<float> vertex2;
};