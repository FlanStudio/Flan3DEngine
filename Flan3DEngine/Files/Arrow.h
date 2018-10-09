#pragma once
#include "Primitive.h"
#include "Globals.h"
#include <vector>

class Arrow : public Primitive
{
public:
	Arrow() : Primitive() { }
	Arrow(float3 pos) : Primitive(pos) { }
	~Arrow();
public:
	void Init();
	void pRender();

private:
	uint indexID = 0;
	uint vertexID = 0;
	std::vector<float> vertex;
	std::vector<uint> index;
};
