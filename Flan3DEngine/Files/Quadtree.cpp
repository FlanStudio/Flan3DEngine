#include "Quadtree.h"
#include "GameObject.h"

#define NW 0
#define NE 1
#define SW 2
#define SE 3

#define LIMIT_GAMEOBJECTS 8

void Quadtree::Create(AABB limits)
{
	if(!root.isInitialized())
		root.Initialize(limits);
}

void Quadtree::Clear()
{
	root.Clear();
}

void Quadtree::Resize(AABB limits)
{
	//Clear, initialize and reinsert all the gameobjects
	std::vector<GameObject*> gameObjects;
	root.getGameObjects(gameObjects);
	Clear();
	Create(limits);
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		Insert(gameObjects[i]);
	}
}

void Quadtree::Insert(const GameObject* go)
{
	if (isWithinLimits(go))
	{
		root.Insert(go);
	}
}

void Quadtree::Remove(const GameObject* go)
{
	std::vector<GameObject*> gameObjects;
	root.getGameObjects(gameObjects);

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i] == go)
		{
			gameObjects.erase(gameObjects.begin() + i);
			break; //The list doesn't have duplicates
		}
	}

	AABB limits = this->root.quad;
	Clear();
	Create(limits);

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		Insert(gameObjects[i]);
	}
}

bool Quadtree::Intersect(std::vector<GameObject*>& result, const Frustum& frustum) const
{
	return root.Intersect(result, frustum);
}

bool Quadtree::isWithinLimits(const GameObject* go) const
{
	return root.quad.Contains(go->boundingBox);
}

void Quadtree::Draw()
{	
	root.Draw();
}

bool QuadtreeNode::AABBContainsFrustum(const AABB& aabb, const Frustum& frustum) const
{
	std::vector<Plane> planes;
	planes.resize(6);
	frustum.GetPlanes(planes.data());

	int numPlanesOutSideAABB = 0;

	for (int i = 0; i < 6; ++i) //For each plane
	{
		std::vector<float3> points;
		points.resize(8);
		aabb.GetCornerPoints(points.data());

		int numPointsOverPlane = 0;
		for (int j = 0; j < 8; ++j) //Check each aabb vertex
		{
			if (planes[i].IsOnPositiveSide(points[j]))
				numPointsOverPlane++;
		}

		if (numPointsOverPlane == 8) //All the points are over the plane
			numPlanesOutSideAABB++;
	}

	if (numPlanesOutSideAABB == 6) //All the planes are outside the AABB
		return false;
	else
		return true;
}

void QuadtreeNode::Draw()
{
	if (!vertex || quadBufferIndex == 0)
	{
		vertex = new float[36 * 3];

		quad.Triangulate(1, 1, 1, (float3*)vertex, nullptr, nullptr, true);

		glGenBuffers(1, &quadBufferIndex);
		glBindBuffer(GL_ARRAY_BUFFER, quadBufferIndex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, quadBufferIndex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, 36);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i].Draw();
	}
}

QuadtreeNode::~QuadtreeNode()
{
	if (vertex || quadBufferIndex == 0)
	{
		delete vertex;
		vertex = nullptr;
		glDeleteBuffers(1, &quadBufferIndex);
	}	
}

void QuadtreeNode::Initialize(AABB quad)
{
	if(!isInitialized())
		this->quad.Enclose(quad);
}

void QuadtreeNode::Clear()
{
	quad.SetNegativeInfinity();
	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i].Clear();
	}
	childs.clear();
	gameObjects.clear();

	if (vertex || quadBufferIndex == 0)
	{
		delete vertex;
		vertex = nullptr;
		glDeleteBuffers(1, &quadBufferIndex);
	}
}

void QuadtreeNode::Insert(const GameObject* go)
{
	gameObjects.push_back((GameObject*)go);

	if (isLeaf()) //No subdivisions yet
	{
		if (gameObjects.size() > LIMIT_GAMEOBJECTS)
		{
			CreateChilds();
			RedistributeGameObjects();
		}
	}
	else   //Already had subdivisions
	{
		RedistributeGameObjects();
	}
}

void QuadtreeNode::CreateChilds()
{
	if (childs.size() == 0)
	{
		childs.resize(4);
		
		float3 center = quad.CenterPoint();
		float halfSize = quad.HalfSize().Length();	

		AABB newAABB;
		
		//NORTH WEST
		newAABB.minPoint = center + float3(-halfSize, -halfSize, 0);
		newAABB.maxPoint = center + float3(0, halfSize, halfSize);
		childs[NW].quad = newAABB;

		//NORTH EAST
		newAABB.minPoint = center + float3(0, -halfSize, 0);
		newAABB.maxPoint = center + float3(halfSize, halfSize, halfSize);
		childs[NE].quad = newAABB;

		//SOUTH WEST
		newAABB.minPoint = center + float3(-halfSize, -halfSize, -halfSize);
		newAABB.maxPoint = center + float3(0, halfSize, 0);
		childs[SW].quad = newAABB;

		//SOURHT EAST
		newAABB.minPoint = center + float3(0, -halfSize, -halfSize);
		newAABB.maxPoint = center + float3(halfSize, halfSize, 0);
		childs[SE].quad = newAABB;
	}
}

void QuadtreeNode::RedistributeGameObjects()
{
	for (std::vector<GameObject*>::iterator itGO = gameObjects.begin(); itGO != gameObjects.end(); /*We will advance manually the for*/)
	{
		GameObject* gameObject = *itGO;
		bool colWith[4] = { false, false, false, false };

		for (int i = 0; i < 4; ++i)
			if (childs[i].quad.Contains(gameObject->boundingBox))
				colWith[i] = true;

		if(colWith[0] && colWith[1] && colWith[2] && colWith[3])
		{
			//Keep the gameObject and advance the for
			++itGO;
		}

		else
		{
			//advance the for and clear this gameobject
			itGO = gameObjects.erase(itGO);
			for (int i = 0; i < 4; ++i)
			{
				if (childs[i].quad.Contains(gameObject->boundingBox))
					childs[i].Insert(gameObject);
			}
		}
	}
}

void QuadtreeNode::getGameObjects(std::vector<GameObject*>& gameObjects) const
{
	for (int i = 0; i < this->gameObjects.size(); ++i)
	{
		bool alreadyInserted = false;

		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (this->gameObjects[i] == gameObjects[j])
				alreadyInserted = true;
		}

		if(!alreadyInserted)
			gameObjects.push_back(this->gameObjects[i]);

	}

	for (int i = 0; i < this->childs.size(); ++i)
	{
		childs[i].getGameObjects(gameObjects);
	}
}

bool QuadtreeNode::Intersect(std::vector<GameObject*>& result, const Frustum& frustum) const
{
	bool ret = false;
	if (AABBContainsFrustum(quad, frustum))									//If the quad contains the frustum maybe our gameobjects also do that
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (AABBContainsFrustum(gameObjects[i]->boundingBox, frustum))	//If this gameobject's aabb contains the frustum
			{
				ret = true;
				
				bool alreadyInserted = false;

				for (int j = 0; j < result.size(); ++j)						//Check if we already have added this gameobject to the vector
				{
					if (gameObjects[i] == result[j])
						alreadyInserted = true;
				}

				if (!alreadyInserted)
					result.push_back(this->gameObjects[i]);					//Add the gameobject to the result vector
		
			}
		}

		for (int i = 0; i < childs.size(); ++i)
		{
			bool childs_ret = childs[i].Intersect(result, frustum);			//Say my childs that maybe they collide with the frustum too
			
			if (ret == false)
				ret = childs_ret;
		}
	}

	return ret;
}
