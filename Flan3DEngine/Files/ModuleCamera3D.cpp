#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib_1.5/Math/float3x3.h"
#include "Brofiler\Brofiler.h"
#include "imgui/imgui.h"

#include "ComponentCamera.h"
#include "GameObject.h"

#define SPEED 100.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module("ModuleCamera3D", start_enabled)
{
	
}

ModuleCamera3D::~ModuleCamera3D()
{}


bool ModuleCamera3D::Init()
{
	Debug.Log("Setting up the camera");

	editorCamera = new GameObject(nullptr);
	editorCamera->CreateComponent(ComponentType::TRANSFORM);
	editorCamComponent = (ComponentCamera*)editorCamera->CreateComponent(ComponentType::CAMERA);
	editorCamComponent->aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
	editorCamComponent->farDistance = 1000.0f;
	editorCamComponent->width = SCREEN_WIDTH;
	editorCamComponent->height = SCREEN_HEIGHT;
	editorCamComponent->horizontalFOV = DegToRad(60);
	editorCamComponent->calculateVerticalFOV();
	editorCamComponent->updateFrustum();


	editorCamera->transform->position = { 0,2,0 };

	activeCamera = editorCamera;
	activeCamComponent = editorCamComponent;

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	bool ret = true;
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	Debug.Log("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update()
{
	BROFILER_CATEGORY("ModuleCamera3D_Update", Profiler::Color::AliceBlue)

	editorCamera->Update(App->time->dt);

	if(!ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered())
		CameraInputs(App->time->dt);

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::CameraInputs(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		AABB bb = App->renderer3D->getSceneAABB();
		center = bb.CenterPoint();

		float3 dir = (editorCamera->transform->position - center).Normalized();

		double sin = Sin(min(SCREEN_WIDTH / SCREEN_HEIGHT, 60.0f * DEGTORAD) * 0.5);
		orbitalRadius = ((bb.Size().MaxElement()) / sin);

		editorCamera->transform->position = (center + dir * orbitalRadius);

		LookAt(center);
	}


	float speed = SPEED * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = SPEED * 5 * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		speed = SPEED * dt;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) 
		editorCamera->transform->position -= float3(0,1,0) * speed;
	
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) 
		editorCamera->transform->position += float3(0, 1, 0) * speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
		editorCamera->transform->position += editorCamera->transform->rotation * float3(0, 0, 1) * speed;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
		editorCamera->transform->position -= editorCamera->transform->rotation * float3(0, 0, 1) * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
		editorCamera->transform->position += editorCamera->transform->rotation * float3(1, 0, 0) * speed;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		editorCamera->transform->position -= editorCamera->transform->rotation * float3(1, 0, 0) * speed;

	if (App->input->GetMouseZ() != 0)
	{
		int wheelY = App->input->GetMouseZ();
		editorCamera->transform->position += editorCamera->transform->rotation * float3(0, 0, 1) * speed * 7 * wheelY;
	}

	// Mouse motion ----------------
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		rotateCamera(dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT &&
		App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		rotateAroundCenter(dt);
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	/*this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();*/
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3 &target)
{
	//Orient the localY axis to the World-Y axis
	float3 localY = editorCamera->transform->rotation * float3(0, 1, 0);
	Quat rotationY = Quat::RotateFromTo(localY, float3(0, 1, 0));
	editorCamera->transform->rotation = rotationY * editorCamera->transform->rotation;

	//Calculate the desired direction for z and get the quaternion that makes the rotation z -> desiredDirection
	float3 zDir = (target - editorCamera->transform->position).Normalized();
	float3 localZ = editorCamera->transform->rotation * float3(0, 0, 1);
	Quat rotation = Quat::RotateFromTo(localZ, zDir);
	editorCamera->transform->rotation = rotation * editorCamera->transform->rotation;
}
// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &movement)
{
	editorCamera->transform->position += movement;
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return editorCamComponent->getViewMatrix().ptr();
}

// -----------------------------------------------------------------

void ModuleCamera3D::rotateCamera(float dt) //Rotating the camera around the World-X, World-Y axis
{
	int dx = -App->input->GetMouseXMotion();
	int dy = App->input->GetMouseYMotion();

	if (dy != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(float3(1, 0, 0), dy * dt);	
		editorCamera->transform->rotation = editorCamera->transform->rotation * rotation;
	}
	if (dx != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(float3(0, 1, 0), dx * dt);
		editorCamera->transform->rotation = editorCamera->transform->rotation * rotation;
	}

}

void ModuleCamera3D::rotateAroundCenter(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 1;

	if (dy != 0)
	{
		float angle = dy * dt;
		Quat rotation = Quat::RotateAxisAngle(float3(1, 0, 0), angle);

		float3 dir = editorCamera->transform->position - center;
		float distance = dir.Length();
		dir.Normalize();

		dir = rotation * dir;

		editorCamera->transform->position = center + (dir * distance);
		editorCamera->transform->rotation = editorCamera->transform->rotation * rotation;
	}
	if (dx != 0)
	{
		float3 rotateValue = float3(0, dx * dt, 0);
		Quat rotation = Quat::FromEulerXYZ(rotateValue.x, rotateValue.y, rotateValue.z);

		float3 dir = editorCamera->transform->position - center;
		float distance = dir.Length();
		dir.Normalize();

		dir = rotation * dir;

		editorCamera->transform->position = center + (dir * distance);
		editorCamera->transform->rotation = editorCamera->transform->rotation * rotation;
	}

	LookAt(center); //Because of that the camera can never have his local-Y axis looking towards the floor. I like the result, but could not be the desirable one.
}

void ModuleCamera3D::OnResize(int w, int h)
{
	editorCamComponent->RecalculateProjectionMatrix(w, h);
}

void ModuleCamera3D::setGameCamera(ComponentCamera* component)
{
	if (component)
	{
		gameCamera = component->gameObject;
		gameCamComponent = component;
	}
	else
	{
		gameCamera = nullptr;
		gameCamComponent = nullptr;
	}
}
