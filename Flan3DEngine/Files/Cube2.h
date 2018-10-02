#pragma once
#include "Primitive.h"
#include "Globals.h"
#include <vector>

class Cube2 : public Primitive
{
public:
	Cube2() : Primitive() { }
	Cube2(float3 pos) : Primitive(pos) { }

public:
	void Init();
	void pRender();
	void Refresh();

private:
	std::vector<float> vertex;
	std::vector<uint> index;
};