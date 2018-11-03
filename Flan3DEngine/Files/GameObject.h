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
		genUUID();
	}
	virtual ~GameObject();
	virtual bool Update(float dt);

public:
	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::string name = "default";
	std::vector<GameObject*> childs;
	AABB boundingBox;

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
	void ClearComponent(Component* component);
	void ClearComponents();

	//Other methods
	void OnInspector();
	
	//AABB drawing data-----------------
	void drawAABB()const;
	void updateAABBbuffers();	
	void destroyAABBbuffers();
	void recursiveDebugDraw()const;

	//----------Serializing methods------------
	void Decompose(std::vector<GameObject*>&, std::vector<ComponentTransform*>&, std::vector<ComponentMesh*>&, std::vector<ComponentCamera*>&);
	
	//UUID, parent UUID, name (Up to 50 chars)
	uint bytesToSerialize() { return sizeof(uint32_t) * 2 + sizeof(uint) + name.length() * sizeof(uint); };
	void Serialize(char*& cursor);
	void DeSerialize(char*& cursor, uint32_t& parentUUID);
	void ReorderComponents();

	//----------------------------------------

private:
	void debugDraw()const;

	//Makes his AABB enclose its ComponentMesh vertex, transformed
	void transformAABB();

	void genUUID()
	{
		if (UUID == 0)
			UUID = FLAN::randomUINT32_Range();
	}

public:
	//Hierarchy's tree utilities
	bool selected = false;
	bool treeOpened = false;

	uint32_t UUID = 0u;
	std::vector<Component*> components;

private:
	bool active = true;
	bool hasMaterial = false;
	
	//AABB drawing data-----------------
	float* AABBvertex = nullptr;
	uint numAABBvertex = 0u;
	uint bufferIndex = 0u;
};

//Manually changing a gameObject's name to 'default' when name is empty
int OnInputCallback(ImGuiInputTextCallbackData* callback);

#endif