#pragma once

#include "MathGeoLib_1.5/MathGeoLib.h"

class Primitive
{
protected:
	//TODO: USE POSITION AND TRANSLATE THE PRIMITIVES
	float3 position = {0,0,0};
	float3 scale = {1,1,1};
	float3 rotation = {0,0,0};
	float4 color = { 0,0,0,1 };
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

	void setScale(float x, float y, float z)
	{
		scale = { x,y,z };
		Refresh();
	}
	
	void Rotate(float angle, float axis_x, float axis_y, float axis_z)
	{
		float3 axis = { axis_x, axis_y, axis_z };
		axis.Normalize();
		rotation = {axis.x * angle, axis.y * angle, axis.z * angle};
	}

	void Translate()
	{
		//TODO: DO SOMETHING HERE
		Refresh();
	}

	void setColor(float4 color)
	{
		this->color = color;
	}

	virtual void Refresh()	{}

protected:
	//Override this method to draw your triangles and stuff
	virtual void pRender()
	{

	}
};
