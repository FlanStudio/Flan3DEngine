#pragma once

#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

#include "Component.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* gameObject) : Component(ComponentType::TRANSFORM, gameObject), position(float3::zero), rotation(Quat::identity), scale(float3::one) {}
	ComponentTransform() {}
public:
	float3 position;
	Quat rotation;
	float3 scale;

public:
	ComponentTransform getGlobal() const;
	ComponentTransform getLocal(ComponentTransform parent) const;
	void setLocal(ComponentTransform parent);
	void OnInspector();
};