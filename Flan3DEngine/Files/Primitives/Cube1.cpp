#include "../Application.h"
#include "Cube1.h"

#include "../Glew/include/glew.h"

Cube1::~Cube1()
{
	glDeleteBuffers(1, &bufferId);
}

void Cube1::Init()
{	
	vertex =
	{
		0,0,0, 1,0,0, 0,1,0, 1,0,0, 1,1,0, 0,1,0,/*face1 FRONT*/
		1,0,0, 1,0,1, 1,1,0, 1,0,1, 1,1,1, 1,1,0, /*face2 RIGHT*/
		0,1,0, 1,1,0, 0,1,1, 1,1,0, 1,1,1, 0,1,1, /*face3 TOP*/
		1,0,1, 0,0,1, 1,1,1, 0,0,1, 0,1,1, 1,1,1,/*face4 BACK*/
		0,0,0, 0,0,1, 1,0,1, 0,0,0, 1,0,1, 1,0,0, /*face5 BOTTOM*/
		0,0,1, 0,0,0, 0,1,1, 0,0,0, 0,1,0, 0,1,1 /*face6 LEFT*/
	};	
	
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube1::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle, axis.x, axis.y, axis.z);
	glScalef(scale.x, scale.y, scale.z);
	glColor4f(color.x, color.y, color.z, color.w);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36*3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glTranslatef(-position.x, -position.y, -position.z);
	glRotatef(-angle, axis.x, axis.y, axis.z);
	glScalef(-scale.x, -scale.y, -scale.z);
}