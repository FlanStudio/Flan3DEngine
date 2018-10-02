#include "Application.h"
#include "Cube1.h"

void Cube1::Init()
{	
	vertex =
	{
		0,0,0, scale.x,0,0, 0,scale.y,0, scale.x,0,0, scale.x,scale.y,0, 0,scale.y,0,/*face1 FRONT*/
		scale.x,0,0, scale.x,0,scale.z, scale.x,scale.y,0, scale.x,0,scale.z, scale.x,scale.y,scale.z, scale.x,scale.y,0, /*face2 RIGHT*/
		0,scale.y,0, scale.x,scale.y,0, 0,scale.y,scale.z, scale.x,scale.y,0, scale.x,scale.y,scale.z, 0,scale.y,scale.z, /*face3 TOP*/
		scale.x,0,scale.z, 0,0,scale.z, scale.x,scale.y,scale.z, 0,0,scale.z, 0,scale.y,scale.z, scale.x,scale.y,scale.z,/*face4 BACK*/
		0,0,0, 0,0,scale.z, scale.x,0,scale.z, 0,0,0, scale.x,0,scale.z, scale.x,0,0, /*face5 BOTTOM*/
		0,0,scale.z, 0,0,0, 0,scale.y,scale.z, 0,0,0, 0,scale.y,0, 0,scale.y,scale.z /*face6 LEFT*/
	};

	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, &vertex[0], GL_STATIC_DRAW);
}

void Cube1::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();
	glRotatef(angle, axis.x, axis.y, axis.z);
	glColor4f(color.x, color.y, color.z, color.w);
	//glRotatef(40, 0, 1, .3);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Cube1::Refresh()
{
	vertex =	
	{
		0,0,0, scale.x,0,0, 0,scale.y,0, scale.x,0,0, scale.x,scale.y,0, 0,scale.y,0,/*face1 FRONT*/
		scale.x,0,0, scale.x,0,scale.z, scale.x,scale.y,0, scale.x,0,scale.z, scale.x,scale.y,scale.z, scale.x,scale.y,0, /*face2 RIGHT*/
		0,scale.y,0, scale.x,scale.y,0, 0,scale.y,scale.z, scale.x,scale.y,0, scale.x,scale.y,scale.z, 0,scale.y,scale.z, /*face3 TOP*/
		scale.x,0,scale.z, 0,0,scale.z, scale.x,scale.y,scale.z, 0,0,scale.z, 0,scale.y,scale.z, scale.x,scale.y,scale.z,/*face4 BACK*/
		0,0,0, 0,0,scale.z, scale.x,0,scale.z, 0,0,0, scale.x,0,scale.z, scale.x,0,0, /*face5 BOTTOM*/
		0,0,scale.z, 0,0,0, 0,scale.y,scale.z, 0,0,0, 0,scale.y,0, 0,scale.y,scale.z /*face6 LEFT*/
	};
}