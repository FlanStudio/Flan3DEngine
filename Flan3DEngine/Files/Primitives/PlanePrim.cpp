#include "../Application.h"
#include "PlanePrim.h"

#define VERTEX_NUM  lenCuts * higCuts
#define INDEX_NUM 2 * 3 * (lenCuts-1) * (higCuts-1)

PlanePrim::~PlanePrim()
{
	glDeleteBuffers(1, &indexID);
	glDeleteBuffers(1, &vertexID);
}

void PlanePrim::Init()
{
	index.resize(INDEX_NUM);
	vertex.resize(VERTEX_NUM *3);

	uint x = lenCuts;
	uint y = x + 1;
	uint z = 0;

	for (uint i = 0, j = 0; i < (higCuts - 1) * 2 * (lenCuts - 1); i++)
	{
		j = i * 3;

		if (!(i % 2 == 0))
		{
			x++;
			y -= lenCuts;
		}
		else if (i % 2 == 0 && i != 0)
		{
			z++;
			y += (lenCuts + 1);
		}

		if (i % (2 * (lenCuts-1)) == 0 && i != 0)
		{
			x++;
			y++;
			z++;
		}

		index[j+2] = x;
		index[j + 1] = y;
		index[j ] = z;
	}
	

	uint l = 0;
	float k = 0.0f;
	for (uint i = 0, j = 0; i < lenCuts *higCuts; i++)
	{
		j = i * 3;
		if (i % lenCuts == 0 && i != 0)
		{
			k = 0.0f;
			l++;
		}
		vertex[j] = -length / 2 + k;
		vertex[j + 1] = 0;
		vertex[j + 2] = higth / 2 - (higth*l /(higCuts-1));
		k += (length / (lenCuts-1));
	}

	
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_NUM * 3, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * INDEX_NUM, index.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PlanePrim::pRender()
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

	glDrawElements(GL_TRIANGLES, INDEX_NUM*3, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glTranslatef(-position.x, -position.y, -position.z);
	glRotatef(-angle, axis.x, axis.y, axis.z);
	glScalef(-scale.x, -scale.y, -scale.z);
}