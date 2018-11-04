#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "MathGeoLib_1.5/MathGeoLib.h"
#include <vector>

class GameObject;

class QuadtreeNode
{
	friend class Quadtree;

public:
	QuadtreeNode() { quad.SetNegativeInfinity();}
	QuadtreeNode(AABB quad) : quad(quad) {}

	void Initialize(AABB quad);
	inline bool isInitialized() const { return quad.IsFinite(); }	
	void Clear();
	void Insert(const GameObject* go);

private:
	inline bool isLeaf() const { return childs.size() == 0; }
	void CreateChilds();
	void RedistributeGameObjects();
	void getGameObjects(std::vector<GameObject*>& gameObjects) const;
	bool Intersect(std::vector<GameObject*>& result, const Frustum& frustum)const;
	bool AABBContainsFrustum(const AABB& aabb, const Frustum& frustum) const;


public:
	AABB quad;
	std::vector<QuadtreeNode> childs;
	std::vector<GameObject*> gameObjects;
};

class Quadtree
{
public:
	QuadtreeNode root;

public:
	void Create(AABB limits);
	void Clear();
	void Resize(AABB limits);
	void Insert(const GameObject* go);
	void Remove(const GameObject* go);
	bool Intersect(std::vector<GameObject*>& result, const Ray& ray)const;
	bool Intersect(std::vector<GameObject*>& result, const Frustum& frustum)const;
	bool isWithinLimits(const GameObject* go) const;

};

#endif