#pragma once
#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class PlanePrim : public Primitive
{
public:
	PlanePrim() : Primitive() { }
	PlanePrim(float3 pos) : Primitive(pos) { }
	~PlanePrim();
public:
	void Init();
	void pRender();

public:
	float length = 10.0f;
	float higth = 10.0f;
	uint lenCuts = 10;
	uint higCuts = 10;

private:
	uint indexID = 0;
	uint vertexID = 0;
	std::vector<float> vertex;
	std::vector<uint> index;
};
