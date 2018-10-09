#include "../Application.h"
#include "GridPrim.h"

#define SIZE_ARRAY (cuts + 1) * 2 * 2 * 3

GridPrim::~GridPrim()
{
	glDeleteBuffers(1, &vertexID);
}

void GridPrim::Init()
{
	vertex.resize(SIZE_ARRAY);

	float separation = cuts / length;

	for (uint i = 0 , j = 0 , k = 0; i < (cuts +1)*2; i++)
	{
		j = i * 3;
		
		vertex[j] = (-length / 2) + separation * k;
		vertex[j + 1] = 0.0f;
		if (i % 2 == 0) 
		{
			vertex[j + 2] = -length / 2;
		}
		else
		{
			vertex[j + 2] = length / 2;
			k++;
		}
	}

	for (uint i = (cuts + 1) * 2, j = i * 3, k = 0; i < (cuts + 1) * 2 *2; i++)
	{
		j = i * 3;

		vertex[j+2] = (-length / 2) + separation * k;
		vertex[j + 1] = 0.0f;
		if (i % 2 == 0)
		{
			vertex[j] = -length / 2;
		}
		else
		{
			vertex[j] = length / 2;
			k++;
		}

	}

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SIZE_ARRAY, vertex.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GridPrim::pRender()
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
