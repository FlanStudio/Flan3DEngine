#pragma once

#include "Primitive.h"
#include "Globals.h"
#include <vector>

class CapsulePrim : public Primitive
{
public:
	CapsulePrim() : Primitive() { }
	CapsulePrim(float3 pos) : Primitive(pos) { }
	~CapsulePrim();

public:
	void Init();
	void pRender();

public:
	float radius = 3.0f;
	float length = 5.0f;
	uint cuts = 20;

private:
	uint vertexID = 0;
	std::vector<float> vertex;

	uint vertex2ID = 0;
	std::vector<float> vertex2;
};