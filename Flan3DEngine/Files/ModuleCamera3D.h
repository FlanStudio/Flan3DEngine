#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib_1.5/MathGeoLib.h"


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	void rotateCamera();
	float* GetViewMatrix();

private:
	void CameraInputs(float dt);
	void CalculateViewMatrix();

public:
	
	float3 X, Y, Z, Position, Reference;
	Color background = {0,0,0,1};

private:

	float4x4 ViewMatrix, ViewMatrixInverse;
	float orbitalRadius = 0.0f;
	float3 center = {0.f,0.0f,0.0f};
};