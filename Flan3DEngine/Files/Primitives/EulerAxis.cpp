#include "../Application.h"
#include "EulerAxis.h"

#define SIZE_ARRAY 3*2

EulerAxis::~EulerAxis()
{
	glDeleteBuffers(1, &vertexID);
	glDeleteBuffers(1, &vertex2ID);
	glDeleteBuffers(1, &vertex3ID);
}

void EulerAxis::Init()
{
	vertex.resize(SIZE_ARRAY);
	vertex2.resize(SIZE_ARRAY);
	vertex3.resize(SIZE_ARRAY);

	vertex = { 0,0,0 ,length,0,0 };
	vertex2 = { 0,0,0 ,0,length,0 };
	vertex3 = { 0,0,0 ,0,0,length };

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_ARRAY, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vertex2ID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex2ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_ARRAY, vertex2.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vertex3ID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex3ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_ARRAY, vertex3.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EulerAxis::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();
	glLineWidth(5.0f);

	//-----------------X---------------------
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, SIZE_ARRAY / 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);


	//------------------Y----------------------
	glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertex2ID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, SIZE_ARRAY / 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);


	//------------------Z----------------------
	glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertex3ID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, SIZE_ARRAY / 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}