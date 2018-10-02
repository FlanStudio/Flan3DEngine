#pragma once

#include "MathGeoLib_1.5/MathGeoLib.h"

class Primitive
{
private:
	float3 position = {0,0,0};
	float3 scale = {1,1,1};
public:

	Primitive()
	{
		
	}

	Primitive(float3 pos) : position(pos) {}

	//Call this method to render your primitive
	void Render() 
	{
		pRender();
	}
	//Override this method to draw your triangles and stuff
	virtual void pRender()
	{

	}
};
