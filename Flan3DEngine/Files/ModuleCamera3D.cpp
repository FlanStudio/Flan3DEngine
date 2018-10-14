#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib_1.5/Math/float3x3.h"
#include "Brofiler\Brofiler.h"
#pragma comment( lib, "Brofiler/ProfilerCore32.lib")

#define SPEED 100.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module("ModuleCamera3D", start_enabled)
{
	CalculateViewMatrix();

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 0.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	Debug.Log("Setting up the camera");
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
update_status ModuleCamera3D::Update(float dt)
{
	BROFILER_CATEGORY("ModuleCamera3D_Update", Profiler::Color::AliceBlue)

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			AABB bb = App->meshes->getSceneAABB();
			center = bb.CenterPoint();

			float3 dir = (Position - center).Normalized();

			double sin = Sin(min(SCREEN_WIDTH / SCREEN_HEIGHT, 60.0f * DEGTORAD) * 0.5);
			orbitalRadius = ((bb.Size().MaxElement()) / sin);

			Position = (center + dir * orbitalRadius);

			LookAt(center);
		}

	float3 newPos(0, 0, 0);
	float speed = SPEED * dt;

	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = SPEED * 5 * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		speed = SPEED * dt;

	if(App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos += Y * speed;
	if(App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos -= Y * speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	if(App->input->GetMouseZ() == 1) newPos -= Z * speed * 3;
	if(App->input->GetMouseZ() == -1) newPos += Z * speed * 3;

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		rotateCamera();
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT &&
		App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		rotateCamera();
		LookAt(center);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix.ptr();
	return nullptr;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
	ViewMatrixInverse = ViewMatrix;
	ViewMatrixInverse.Inverse();
}

void ModuleCamera3D::rotateCamera()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float Sensitivity = 0.01f;

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		float3x3 rotMatrixY;
		rotMatrixY = rotMatrixY.RotateY(DeltaX);

		X = X * rotMatrixY;

		Y = Y * rotMatrixY;

		Z = Z * rotMatrixY;
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;
		float3x3 rot;
		rot = rot.RotateAxisAngle(X, DeltaY);
		Y = Y * rot;
		Z = Z * rot;

		if (Y.y < 0.0f)
		{
			Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = Z.Cross(X);
		}
	}

	Position = Reference + Z * Position.Length();
	
}