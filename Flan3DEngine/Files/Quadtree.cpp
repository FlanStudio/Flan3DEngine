#include "Quadtree.h"
#include "GameObject.h"

#include "Brofiler\Brofiler.h"

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
	BROFILER_CATEGORY("QuadtreeResize", Profiler::Color::Azure)

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

void Quadtree::Remove(const std::vector<GameObject*> go)
{
	std::vector<GameObject*> gameObjects;
	root.getGameObjects(gameObjects);

	for (int j = 0; j < go.size(); j++)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (gameObjects[i] == go[j])
			{
				gameObjects.erase(gameObjects.begin() + i);
				break; //The list doesn't have duplicates
			}
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

bool Quadtree::Intersect(std::vector<GameObject*>& result, const LineSegment& segment) const
{
	return root.Intersect(result, segment);
}

bool Quadtree::Intersect(std::vector<GameObject*>& result, const Frustum& frustum) const
{
	return root.Intersect(result, frustum);
}

bool Quadtree::isWithinLimits(const GameObject* go) const
{
	BROFILER_CATEGORY("QuadtreeLimits", Profiler::Color::Azure)

	return root.quad.Intersects(go->boundingBox) || root.quad.Contains(go->boundingBox);
}

void Quadtree::Draw()
{	
	BROFILER_CATEGORY("QuadtreeDraw", Profiler::Color::Azure)

	root.Draw();
}

bool QuadtreeNode::AABBContainsFrustum(const AABB& aabb, const Frustum& frustum) const
{
	BROFILER_CATEGORY("QuadtreeContains", Profiler::Color::Azure)

	std::vector<Plane> planes;
	planes.resize(6);
	frustum.GetPlanes(planes.data());

	for (int i = 0; i < 6; ++i) //For each plane
	{
		std::vector<float3> points;
		points.resize(8);
		aabb.GetCornerPoints(points.data());

		int numPointsUnderPlane = 8;
		for (int j = 0; j < 8; ++j) //Check each aabb vertex
		{
			if (planes[i].IsOnPositiveSide(points[j]))
			{
				numPointsUnderPlane--;
			}
		}

		if (numPointsUnderPlane == 0) //All the points are over the plane
			return false;
	}
	return true;
}

void QuadtreeNode::Draw()
{
	BROFILER_CATEGORY("QuadtreeDraw", Profiler::Color::Azure)

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
	if (!isInitialized())
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
		if (gameObjects.size() >= LIMIT_GAMEOBJECTS)
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
		float3 size = quad.Size();
		float3 newSize = { size.x*0.5f, size.y, size.z*0.5f };

		float3 newCenter = center;
		
		//NORTH WEST
		newCenter.x = center.x - size.x * 0.25;
		newCenter.z = center.z + size.z * 0.25f;
		childs[NW].quad.SetFromCenterAndSize(newCenter, newSize);

		//NORTH EAST
		newCenter.x = center.x + size.x * 0.25;
		newCenter.z = center.z + size.z * 0.25f;
		childs[NE].quad.SetFromCenterAndSize(newCenter, newSize);

		//SOUTH WEST
		newCenter.x = center.x - size.x * 0.25;
		newCenter.z = center.z - size.z * 0.25f;
		childs[SW].quad.SetFromCenterAndSize(newCenter, newSize);

		//SOURHT EAST
		newCenter.x = center.x + size.x * 0.25;
		newCenter.z = center.z - size.z * 0.25f;
		childs[SE].quad.SetFromCenterAndSize(newCenter, newSize);
	}
}

void QuadtreeNode::RedistributeGameObjects()
{
	BROFILER_CATEGORY("QuadtreeRedistribute", Profiler::Color::Azure)

	for (std::vector<GameObject*>::iterator itGO = gameObjects.begin(); itGO != gameObjects.end(); /*We will advance manually the for*/)
	{
		GameObject* gameObject = *itGO;
		bool colWith[4] = { false, false, false, false };

		for (int i = 0; i < 4; ++i)
		{
			if (childs[i].quad.Contains(gameObject->boundingBox) || childs[i].quad.Intersects(gameObject->boundingBox))
				colWith[i] = true;
		}
			

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
				if (childs[i].quad.Contains(gameObject->boundingBox) || childs[i].quad.Intersects(gameObject->boundingBox))
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

bool QuadtreeNode::Intersect(std::vector<GameObject*>& result, const LineSegment& segment) const
{
	bool ret = false;
	if (quad.Intersects(segment))
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (gameObjects[i]->boundingBox.Intersects(segment))
			{
				ret = true;

				bool alreadyInserted = false;

				for (int j = 0; j < result.size(); ++j)						
				{
					if (gameObjects[i] == result[j])
						alreadyInserted = true;
				}

				if (!alreadyInserted)
					result.push_back(this->gameObjects[i]);
			}
		}

		for (int i = 0; i < childs.size(); ++i)
		{
			bool childs_ret = childs[i].Intersect(result, segment);			

			if (ret == false)
				ret = childs_ret;
		}
	}
	return ret;
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
