#pragma once

#include "MathGeoLib_1.5/Math/float3.h"
#include "MathGeoLib_1.5/Math/Quat.h"

class Transform
{
	float3 position;
	Quat rotation;
	float3 scale;
};