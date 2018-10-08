#include "Application.h"
#include "RayPrim.h"

#define SIZE_ARRAY 2*3

RayPrim::~RayPrim()
{
	glDeleteBuffers(1, &vertexID);
}

void RayPrim::Init()
{
	vertex.resize(SIZE_ARRAY);

	for (int i = 0; i < SIZE_ARRAY; i++)
	{
		vertex[i] = points[i];
	}

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_ARRAY, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RayPrim::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, SIZE_ARRAY / 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);


	glLineWidth(1.0f);
}