#include "../Application.h"
#include "CapsulePrim.h"

CapsulePrim::~CapsulePrim()
{
	glDeleteBuffers(1, &vertexID);

	glDeleteBuffers(1, &vertex2ID);
}

void CapsulePrim::Init()
{
	assert(radius > 0 && cuts > 0);
	assert(cuts % 2 == 0);

	vertex.resize((cuts *2 +4) * 3);
	vertex2.resize((cuts *2 +4)* 3);

	float angle = 0.0f;
	float angle_freq = 360.0f / (float)cuts;

	radius *= 0.25f;
	int j = 0;

	for (uint i = 0; i < (cuts *2) +4; i++, angle += angle_freq)
	{
		if (((i % 2)==0 && i != 0) || i == cuts + 1 || i == cuts + 2 || i == (cuts * 2) + 2 || i == (cuts * 2) + 3)
		{
			angle -= angle_freq;
		}

		j = i * 3;
		vertex[j] = (cos(angle * DEGTORAD) * radius);
		vertex[j+1] = (sin(angle * DEGTORAD) * radius);
		vertex[j + 2] =(0.0f);

		vertex2[j] = (cos(angle * DEGTORAD) * radius);
		vertex2[j + 1] = (sin(angle * DEGTORAD) * radius);
		vertex2[j + 2] = (0.0f);

		if (i > cuts && i != (cuts * 2 +4) - 1)
		{
			vertex[j+1] -= (length);
			vertex2[j+1] -= (length);
		}
	}
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (cuts * 2 + 4) * 3, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vertex2ID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex2ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) *(cuts * 2 + 4) * 3, vertex2.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CapsulePrim::pRender()
{
	float angle = rotation.Length();
	float3 axis = rotation.Normalized();

	//----------------CIRCLE1-----------------------
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, (cuts * 2 + 4));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	//-------------------CIRCLE2-----------------------
	glRotatef(90, 0, 1, 0);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertex2ID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, (cuts * 2 + 4));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glRotatef(-90, 0, 1, 0);

	glLineWidth(1.0f);
}
