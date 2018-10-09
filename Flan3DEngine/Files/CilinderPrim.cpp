#include "Application.h"
#include "CilinderPrim.h"

#define VERTEX_NUM ((cuts+1) * 2)
#define INDEX_NUM cuts * 4 * 3

CilinderPrim::~CilinderPrim()
{
	glDeleteBuffers(1, &indexID);
	glDeleteBuffers(1, &vertexID);
}


void CilinderPrim::Init()
{
	index.resize(INDEX_NUM );
	vertex.resize(VERTEX_NUM * 3);

	float angle = 0.0f, angle_inc = 360 / cuts,x = 0, y = 0, z = 0;
	for (uint i = 0, j = 0; i < VERTEX_NUM; i++)
	{
		j = i * 3;

		if (i == 0 || i == cuts + 1)
		{
			x = z = 0;
			y = higth / 2;
			angle = 0.0f;
		}
		else
		{
			x = cos(angle * DEGTORAD) * Radius;
			y = higth / 2;
			z = sin(angle * DEGTORAD) * Radius;
			angle += angle_inc;
		}
		if (i > cuts)
		{
			y -= higth;
		}

		vertex[j] = x;
		vertex[j + 1] = y;
		vertex[j + 2] = z;
	}

	for (uint i = 0, x = 0, y = 0, z = cuts - 1, k = 0, l = 0, j = 0; i < INDEX_NUM/3; i++)
	{
		j = i * 3;

		if (i < cuts)
		{
			y++;
			z++;
			if (i == 1)
			{
				z -= cuts;
			}
		}
		else if (i >= cuts * 3)
		{
			if (l == 0)
			{
				x = cuts + 1;
				y = cuts + 1;
				z = y + 1;
			}
			else if (l == cuts - 1)
			{
				z = cuts + 1;
			}
			y++;
			z++;
			l++;
		}
		else
		{
			if (k == 0)
			{
				x = 1;
				z = cuts + 2;
				y = z + 1;
			}
			else if (k % 2 == 0)
			{
				x++;
				y += cuts + 2;
			}
			else
			{
				z++;
				y -= cuts + 1;
			}
			if (k == (cuts * 2) - 2)
			{
				y = cuts + 2;
			}
			else if (k == (cuts * 2) - 1)
			{
				z = cuts + 2;
			}
			k++;
		}

		index[j] = x;
		index[j + 1] = y;
		index[j + 2] = z;
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

void CilinderPrim::pRender()
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

	glDrawElements(GL_TRIANGLES, INDEX_NUM * 3, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glTranslatef(-position.x, -position.y, -position.z);
	glRotatef(-angle, axis.x, axis.y, axis.z);
	glScalef(-scale.x, -scale.y, -scale.z);
}