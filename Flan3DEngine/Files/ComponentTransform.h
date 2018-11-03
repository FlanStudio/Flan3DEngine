#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "MathGeoLib_1.5/MathGeoLib.h"
#include "Component.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* gameObject) : Component(ComponentType::TRANSFORM, gameObject), position(float3::zero), rotation(Quat::identity), scale(float3::one) {}
	ComponentTransform() : Component(ComponentType::TRANSFORM, nullptr) {}
public:
	float3 position;
	Quat rotation;
	float3 scale;

public:
	ComponentTransform getGlobal() const;
	ComponentTransform getLocal(ComponentTransform* newParent) const;
	void setLocalWithParent(ComponentTransform* newParent);
	void setLocal(ComponentTransform* newTransform);
	void setLocalWithParentGlobal(ComponentTransform parentGlobal);
	void OnInspector();
	float4x4 getMatrix()const;

	//GameObject's UUID, Position scale rotation 
	static uint bytesToSerialize() { return sizeof(uint32_t) + sizeof(float) * 3 * 2 + sizeof(float) * 4; }
	void Serialize(char*& cursor)const;
	void DeSerialize(char*& cursor, uint32_t& goUUID);
};

#endif