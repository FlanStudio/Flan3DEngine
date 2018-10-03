#include "Application.h"
#include "Cube2.h"

void Cube2::Init()
{
	index.resize(36);
	vertex.resize(8 * 3);
	index = { 0,1,3,1,2,3, 1,5,2,5,6,2, 4,0,7,0,3,7, 4,7,6,4,6,5, 3,2,7,2,6,7, 0,4,1,4,5,1 };
	vertex = { 0,0,0, scale.x,0,0, scale.x,scale.y,0, 0,scale.y,0, 0,0,scale.z, scale.x,0,scale.z, scale.x,scale.y,scale.z, 0,scale.y,scale.z };

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, &index[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, &vertex[0], GL_STATIC_DRAW);

}

void Cube2::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();
	glRotatef(angle, axis.x, axis.y, axis.z);
	glColor4f(color.x, color.y, color.z, color.w);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_BYTE, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Cube2::Refresh()
{
	vertex = { 0,0,0, scale.x,0,0, scale.x,scale.y,0, 0,scale.y,0, 0,0,scale.z, scale.x,0,scale.z, scale.x,scale.y,scale.z, 0,scale.y,scale.z };
}





