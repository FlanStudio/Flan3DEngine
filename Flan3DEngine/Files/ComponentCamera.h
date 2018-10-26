#pragma once

#include "Component.h"
#include "MathGeoLib_1.5/MathGeoLib.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, bool active = true);
	void OnInspector();
	void RecalculateProjectionMatrix(int w, int h);
	float4x4 getViewMatrix();
private:	 
	Frustum frustum;

	ImColor backgroundColor = {1,1,1,1};
	float nearDistance = 0.0f;
	float farDistance = 0.0f;

	float aspectRatio = 0.0f;		// w:h
	float verticalFOV = 0.0f;		//fixed
	float horizontalFOV = 0.0f;		//we adjust that: 60-90

	bool isMainCamera = true;

};
