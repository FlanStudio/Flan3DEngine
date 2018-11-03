#ifndef __COMPONENTCAMERA_H__
#define __COMPONENTCAMERA_H__

#include "Component.h"
#include "MathGeoLib_1.5/MathGeoLib.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, bool active = true);
	~ComponentCamera();
	bool Update(float dt);

	void OnInspector();
	void RecalculateProjectionMatrix(int w, int h);
	float4x4 getViewMatrix();
	void updateFrustum();
	void calculateVerticalFOV();
	static uint bytesToSerialize() { return sizeof(ImVec4) + sizeof(float) * 7 + sizeof(bool); }
	void Serialize(char* cursor) const;

private:
	void debugDraw() override;

public:
	ImVec4 backgroundColor = { 1,1,1,1 };
	float nearDistance = 0.0f;
	float farDistance = 0.0f;
	float aspectRatio = 0.0f;			
	float width = 0.0f, height = 0.0f;
	float horizontalFOV = 0.0f;
	bool isMainCamera = true;
private:	 
	float verticalFOV = 0.0f;
	Frustum frustum;

	//Drawing the frustum
	uint indexID = 0u;
	uint index[24] = 
	{
		0,2,2,6,6,4,4,0,
		0,1,2,3,6,7,4,5,
		1,3,3,7,7,5,5,1
	};

	float* vertex = nullptr;
	uint vertexID = 0u;
};

#endif
