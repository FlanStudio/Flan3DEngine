#pragma once

#include "Primitive.h"
#include "Globals.h"
#include <vector>

class EulerAngles : public Primitive
{
public:
	EulerAngles() : Primitive() { }
	EulerAngles(float3 pos) : Primitive(pos) { }
	~EulerAngles();

public:
	void Init();
	void pRender();

public:
	float lenght = 1.5f;

private:
	uint vertexID = 0;
	std::vector<float> vertex;

	uint vertex2ID = 0;
	std::vector<float> vertex2;

	uint vertex3ID = 0;
	std::vector<float> vertex3;

};

