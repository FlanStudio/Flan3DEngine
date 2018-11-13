#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include <string>

#include "Component.h"
#include "ComponentTransform.h"

#include "MathGeoLib_1.5/Math/float3x3.h"

class GameObject
{
public:
	GameObject(GameObject* parent) : parent(parent)
	{
		boundingBox.SetNegativeInfinity();
		initialAABB.SetNegativeInfinity();
		genUUID();
	}
	virtual ~GameObject();
	virtual bool Update(float dt);

public:
	//Child-related methods
	void AddChild(GameObject* child);
	void InsertChild(GameObject* child, int pos);
	void ClearChild(GameObject* child);
	void ClearChilds();
	bool HasChilds() const;
	GameObject* getSelectedGO() const;
	void deleteSelected();
	int getChildPos(const GameObject* child) const;

	//Component-related methods
	Component* CreateComponent(ComponentType type);
	void AddComponent(Component* component);	
	void InsertComponent(Component* component, int pos);
	Component* getComponentByType(ComponentType type) const;	
	void deleteComponent(Component* component);
	void ClearComponent(Component* component);
	void ClearComponents();

	//Other methods
	void OnInspector();
	
	//AABB drawing data-----------------
	void drawAABB()const;
	void updateAABBbuffers();	
	void encloseParentAABB();
	void destroyAABBbuffers();
	void recursiveDebugDraw()const;
	void initAABB();
	void updateInitAABB();
	AABB getAABBChildsEnclosed();

	//----------Serializing methods------------
	void Decompose(std::vector<GameObject*>&, std::vector<ComponentTransform*>&, std::vector<ComponentMesh*>&, std::vector<ComponentCamera*>&);
	
	//UUID, parent UUID, name (Up to 50 chars)
	uint bytesToSerialize() { return sizeof(uint32_t) * 2 + sizeof(uint) + name.length() * sizeof(uint); };
	void Serialize(char*& cursor);
	void DeSerialize(char*& cursor, uint32_t& parentUUID);
	void ReorderComponents();

	//----------------------------------------

	//Makes his AABB enclose its ComponentMesh vertex, transformed
	void transformAABB();

private:
	void debugDraw()const;

	void genUUID()
	{
		if (uuid == 0)
			uuid = FLAN::randomUINT32_Range();
	}

public:
	//Hierarchy's tree utilities
	bool selected = false;
	bool treeOpened = false;
	bool drawAABBs = true;

	UID uuid = 0u;
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::string name = "default";
	std::vector<GameObject*> childs;
	AABB boundingBox;

private:
	bool active = true;
	bool hasMaterial = false;
	
	//AABB drawing data-----------------
	float* AABBvertex = nullptr;
	uint numAABBvertex = 0u;
	uint bufferIndex = 0u;
	AABB initialAABB;

	//Components
	char* names[4] = { "Transform","Mesh","Material","Camera" };	//Grow when we have more components
	bool toggles[4] = { true,false,false,false };					//Grow when we have more components
};

//Manually changing a gameObject's name to 'default' when name is empty
int OnInputCallback(ImGuiInputTextCallbackData* callback);

#endif