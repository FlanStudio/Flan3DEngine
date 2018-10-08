#include "Application.h"
#include "Arrow.h"

Arrow::~Arrow()
{
	glDeleteBuffers(1, &indexID);
	glDeleteBuffers(1, &vertexID);
}

void Arrow::Init()
{
	index = { 0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14, 15,16,17 };
	vertex = { 0,-2,0, 1,0,0, 0,2,0, -2,-1,0, 0,-1,0, 0,1,0, -2,-1,0, 0,1,0, -2,1,0,
			   0,0,-2, 1,0,0, 0,0,2, -2,0,-1, 0,0,-1, 0,0,1, -2,0,-1, 0,0,1, -2,0,1 };


	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18 * 3, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 18, index.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Arrow::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle, axis.x, axis.y, axis.z);
	glScalef(scale.x, scale.y, scale.z);
	glColor4f(color.x, color.y, color.z, color.w);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glTranslatef(-position.x, -position.y, -position.z);
	glRotatef(-angle, axis.x, axis.y, axis.z);
	glScalef(-scale.x, -scale.y, -scale.z);
}
