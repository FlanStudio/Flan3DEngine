#pragma once

#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

#include "Component.h"

class ComponentTransform : public Component
{
	float3 position;
	Quat rotation;
	float3 scale;
};