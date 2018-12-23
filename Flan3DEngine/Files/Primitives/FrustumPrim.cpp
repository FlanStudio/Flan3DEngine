#include "../Application.h"
#include "FrustumPrim.h"

FrustumPrim::~FrustumPrim()
{
	glDeleteBuffers(1, &indexID);
	glDeleteBuffers(1, &vertexID);
}

void FrustumPrim::Init()
{
	float a = firstSquareHigh;//First square height
	float b = firstSquareLength;//First square length
	float c = secondSquareHigh;//Second square height
	float d = secondSquareLength;//Second square length
	float e = frustumDepth;//Frustum depth
	float f = (c - a) / 2;
	float g = (d - b) / 2;

	index = { 0,1,3,1,2,3, 1,5,2,5,6,2, 4,0,7,0,3,7, 4,7,6,4,6,5, 3,2,7,2,6,7, 0,4,1,4,5,1 };
	vertex = { 0,0,0, b,0,0, b,a,0, 0,a,0, 0-g,0-f,e, b+g,0-f,e, b+g,a+f,e, 0-g,a+f,e };

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, index.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void FrustumPrim::pRender()
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

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glTranslatef(-position.x, -position.y, -position.z);
	glRotatef(-angle, axis.x, axis.y, axis.z);
	glScalef(-scale.x, -scale.y, -scale.z);
}





