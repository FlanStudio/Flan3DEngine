#pragma once

#include "Primitive.h"
#include "Globals.h"
#include <vector>

class Cube1 : public Primitive
{
public:
	Cube1() : Primitive() { }
	Cube1(float3 pos) : Primitive(pos) { }
	~Cube1();

public:
	void Init();
	void pRender();

private:
	
	uint bufferId = 0;
	std::vector<float> vertex;
};


