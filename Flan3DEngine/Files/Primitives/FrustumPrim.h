#pragma once
#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class FrustumPrim : public Primitive
{
public:
	FrustumPrim() : Primitive() { }
	FrustumPrim(float3 pos) : Primitive(pos) { }
	~FrustumPrim();
public:
	void Init();
	void pRender();

public:
	float firstSquareHigh = 1.0f;
	float firstSquareLength = 2.0f;
	float secondSquareHigh = 3.0f;
	float secondSquareLength = 6.0f;
	float frustumDepth = 5.0f;

private:
	uint indexID = 0;
	uint vertexID = 0;
	std::vector<float> vertex;
	std::vector<uint> index;
};
