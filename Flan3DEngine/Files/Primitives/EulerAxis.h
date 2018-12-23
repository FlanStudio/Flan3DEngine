#pragma once

#include "Primitive.h"
#include "../Globals.h"
#include <vector>

class EulerAxis : public Primitive
{
public:
	EulerAxis() : Primitive() { }
	EulerAxis(float3 pos) : Primitive(pos) { }
	~EulerAxis();

public:
	void Init();
	void pRender();

public:
	float length = 1.5f;

private:
	uint vertexID = 0;
	std::vector<float> vertex;

	uint vertex2ID = 0;
	std::vector<float> vertex2;

	uint vertex3ID = 0;
	std::vector<float> vertex3;

};

