#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

#include <string>

class ResourceMesh;

class ComponentMesh : public Component
{
public:

	ComponentMesh(GameObject* parent, bool active = true) : Component(ComponentType::MESH, parent, active){}
	~ComponentMesh();

	void OnInspector() override;

public:
	void updateGameObjectAABB();

	//Call mesh->drawNormals
	void drawNormals();

	void Serialize(char*& cursor) const override;
	void DeSerialize(char*& cursor, uint32_t& goUUID);
	inline static uint bytesToSerialize() { return sizeof(UID) * 3; } //Your uid, your gameObject uid, your Mesh uid
	void Draw();
	void UpdateNormalsLenght(uint newLenght);

private:
	void genNormalLines();

public:
	ResourceMesh* mesh = nullptr;

private:
	bool meshClicked = false;
};

#endif