#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <vector>
#include <string>

#include "Component.h"

#include "MathGeoLib_1.5/Math/float3x3.h"

#define NUM_COMPONENTS 4

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
	int getChildPos(const GameObject* child) const;

	//Component-related methods
	Component* CreateComponent(ComponentType type);
	void AddComponent(Component* component);	
	void InsertComponent(Component* component, int pos);
	Component* getComponentByType(ComponentType type) const;	
	void deleteComponent(Component* component);
	void ClearComponent(Component* component);
	void ClearComponentAt(int i);
	void ClearComponents();
	int getComponentIndex(const Component* component) const;

	//Other methods
	void OnInspector();
	void ReceiveEvent(Event event);
	void SetActive(bool boolean);
	
	//AABB drawing data-----------------
	void drawAABB()const;
	void updateAABBbuffers();	
	void destroyAABBbuffers();
	void recursiveDebugDraw()const;
	void initAABB();
	AABB getAABBChildsEnclosed();

	//----------Serializing methods------------
	void Decompose(std::vector<GameObject*>&, std::vector<ComponentTransform*>&, std::vector<ComponentMesh*>&, std::vector<ComponentCamera*>&, std::vector<ComponentMaterial*>&);
	
	//UUID, parent UUID, name (Up to 50 chars)
	uint bytesToSerialize() { return sizeof(uint32_t) * 2 + sizeof(uint) + name.length() * sizeof(char); };
	void Serialize(char*& cursor);
	void DeSerialize(char*& cursor, uint32_t& parentUUID);
	void ReorderComponents();

	//----------------------------------------

	//Makes his AABB enclose its ComponentMesh vertex, transformed
	void transformAABB();

	//Gen the UUID
	void genUUID()
	{
		uuid = FLAN::randomUINT32_Range();
	}

private:
	void debugDraw()const;	

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
	AABB initialAABB;

private:
	bool active = true;
	bool hasMaterial = false;
	
	//AABB drawing data-----------------
	float* AABBvertex = nullptr;
	uint numAABBvertex = 0u;
	uint bufferIndex = 0u;

	//Components
	char* names[NUM_COMPONENTS] = { "Transform","Mesh","Material","Camera" };	//Grow when we have more components
	bool toggles[NUM_COMPONENTS] = { true,false,false,false };					//Grow when we have more components
};

//Manually changing a gameObject's name to 'default' when name is empty
int OnInputCallback(ImGuiInputTextCallbackData* callback);

#endif