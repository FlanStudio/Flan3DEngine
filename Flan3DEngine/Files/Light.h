
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Color.h"


struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	//vec3 position;

	int ref;
	bool on;
};

#endif