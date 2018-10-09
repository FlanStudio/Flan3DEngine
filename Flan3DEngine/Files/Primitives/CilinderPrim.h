#pragma once
#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class CilinderPrim : public Primitive
{
public:
	CilinderPrim() : Primitive() { }
	CilinderPrim(float3 pos) : Primitive(pos) { }
	~CilinderPrim();
public:
	void Init();
	void pRender();

public:
	float Radius = 5.0f;
	float higth = 10.0f;
	uint cuts = 20;

private:
	uint indexID = 0;
	uint vertexID = 0;
	std::vector<float> vertex;
	std::vector<uint> index;
};

