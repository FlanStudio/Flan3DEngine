#ifndef __MODULECAMERA3D_H__
#define __MODULECAMERA3D_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib_1.5/MathGeoLib.h"

class GameObject;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	update_status Update();
	update_status PreUpdate();

	bool CleanUp();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& target);
	void Move(const float3& Movement);
	void rotateCamera(float dt);
	void rotateAroundCenter(float dt);
	float4x4 GetViewMatrix();
	float4x4 GetProjMatrix();
	void OnResize(int w, int h);
	void setGameCamera(ComponentCamera* component);

private:
	void CameraInputs(float dt);
	void MousePicking() const;

public:
	GameObject* activeCamera = nullptr;
	ComponentCamera* activeCamComponent = nullptr;

	GameObject* editorCamera = nullptr;
	ComponentCamera* editorCamComponent = nullptr;

	GameObject* gameCamera = nullptr;
	ComponentCamera* gameCamComponent = nullptr;
	
private:
	

	float orbitalRadius = 0.0f;
	float3 center = {0.f,0.0f,0.0f};
};

#endif