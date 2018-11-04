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

void Quadtree::Insert(GameObject * go)
{
	if (isWithinLimits(go))
	{
		root.Insert(go);
	}
}

bool Quadtree::isWithinLimits(GameObject * go) const
{
	return root.quad.Contains(go->boundingBox);
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
}

void QuadtreeNode::Insert(GameObject* go)
{
	gameObjects.push_back(go);

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
		newAABB.minPoint = {center + float3(-halfSize, 0, 0)};
		newAABB.maxPoint = { center + float3(0, 0, halfSize) };
		childs[NW].quad = newAABB;

		//NORTH EAST
		newAABB.minPoint = center;
		newAABB.maxPoint = { center + float3(halfSize, 0, halfSize) };
		childs[NE].quad = newAABB;

		//SOUTH WEST
		newAABB.minPoint = { center + float3(-halfSize, 0, -halfSize) };
		newAABB.maxPoint = center;
		childs[SW].quad = newAABB;

		//SOURHT EAST
		newAABB.minPoint = { center + float3(0, 0, -halfSize) };
		newAABB.maxPoint = { center + float3(halfSize, 0, 0) };
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
