#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib_1.5/Math/float3x3.h"
#include "Brofiler\Brofiler.h"
#include "imgui/imgui.h"
#include "ModuleScene.h"

#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"


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
	editorCamComponent->backgroundColor = { 0,0,0,0 };

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

update_status ModuleCamera3D::PreUpdate()
{
	BROFILER_CATEGORY("Camera3DPreUpdate", Profiler::Color::Azure)

	MousePicking();

	return update_status::UPDATE_CONTINUE;
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

	if (activeCamera == editorCamera)
	{
		editorCamera->Update(App->time->dt);

		if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered())
			CameraInputs(App->time->dt);
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::CameraInputs(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		GameObject* selected = App->scene->getSelectedGO();

		if (selected && selected->boundingBox.IsFinite())
		{
			center = selected->boundingBox.CenterPoint();
			float3 dir = (editorCamera->transform->position - center).Normalized();

			double sin = Sin(min(SCREEN_WIDTH / SCREEN_HEIGHT, 60.0f * DEGTORAD) * 0.5);
			orbitalRadius = ((selected->boundingBox.Size().MaxElement()) / sin);

			editorCamera->transform->position = (center + dir * orbitalRadius);
			LookAt(selected->boundingBox.CenterPoint());
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (App->scene->getRootNode() != nullptr)
		{
			AABB sceneBB = App->scene->getRootNode()->getAABBChildsEnclosed();
			if (sceneBB.IsFinite())
			{
				center = sceneBB.CenterPoint();
				float3 dir = (editorCamera->transform->position - center).Normalized();

				double sin = Sin(min(SCREEN_WIDTH / SCREEN_HEIGHT, 60.0f * DEGTORAD) * 0.5);
				orbitalRadius = ((sceneBB.Size().MaxElement()) / sin);

				editorCamera->transform->position = (center + dir * orbitalRadius);
				LookAt(sceneBB.CenterPoint());
			}
		}
	}

	float speed = SPEED * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = SPEED * 5 * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		speed = SPEED * dt;

	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_REPEAT) 
		editorCamera->transform->position -= float3(0,1,0) * speed;
	
	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT) 
		editorCamera->transform->position += float3(0, 1, 0) * speed;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) 
		editorCamera->transform->position += editorCamera->transform->rotation * float3(0, 0, 1) * speed;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) 
		editorCamera->transform->position -= editorCamera->transform->rotation * float3(0, 0, 1) * speed;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) 
		editorCamera->transform->position += editorCamera->transform->rotation * float3(1, 0, 0) * speed;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
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

void ModuleCamera3D::MousePicking() const
{
	if (activeCamComponent == editorCamComponent) //Disable mouse picking in the Game view
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !App->editor->isSomeGUIHovered()) //Ensure you're not clicking the gui
		{
			float2 mousePos = { (float)App->input->GetMouseX(), (float)App->input->GetMouseY() };

			const Frustum frustum = editorCamComponent->getFrustum();
			mousePos = frustum.ScreenToViewportSpace(mousePos, SCREEN_WIDTH, SCREEN_HEIGHT);	//Normalize mouse position into viewport coordinates

			LineSegment segment = frustum.UnProjectLineSegment(mousePos.x, mousePos.y); //Get the ray from the frustum

			std::vector<GameObject*> intersected;
			App->scene->quadtree.Intersect(intersected, segment); //Get all the gameobjects stored in the quadtree that collides with the ray

			uint numIterations = 0;
			do                                                    //Order the gameobjects by their aabb's distance (they are unique in the provided list)
			{
				numIterations = 0;
				for (int i = 0; i < intersected.size(); ++i)
				{
					GameObject* nextGO = i + 1 < intersected.size() ? intersected[i + 1] : nullptr;
					GameObject* currentGO = intersected[i];

					if (nextGO && currentGO->boundingBox.Distance(segment.a) > nextGO->boundingBox.Distance(segment.a))
					{
						numIterations++;
						intersected.erase(intersected.begin() + i);
						intersected.insert(intersected.begin() + i + 1, currentGO);
					}
				}

			} while (numIterations > 0);

			bool hit = false;
			float distance = inf;
			GameObject* clickedGO = nullptr;

			for (int i = 0; i < intersected.size(); ++i) //For each gameObject
			{
				ComponentMesh* componentMesh = (ComponentMesh*)intersected[i]->getComponentByType(ComponentType::MESH);
				if (!componentMesh)
					continue;

				LineSegment localSpaceSegment(segment);  localSpaceSegment.Transform(intersected[i]->transform->getGlobalMatrix().Transposed().Inverted());

				for (int tri = 0; tri < componentMesh->mesh->num_index / 3; ++tri) //For each triangle in the mesh
				{
					float3 vertices[3] =
					{
						{componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3] * 3], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3] * 3 + 1], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3] * 3 + 2]},
						{componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 1] * 3], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 1] * 3 + 1], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 1] * 3 + 2]},
						{componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 2] * 3], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 2] * 3 + 1], componentMesh->mesh->vertex[componentMesh->mesh->index[tri * 3 + 2] * 3 + 2]}
					};

					Triangle actualTri(vertices[0], vertices[1], vertices[2]);
					float newDistance = 0;
					if (localSpaceSegment.Intersects(actualTri, &newDistance, nullptr))			//if the segment intersects with that triangle, compare the hitpoints
					{
						if (IsFinite(distance))
						{
							if (distance > newDistance) //Keep the closest hitpoint
							{
								distance = newDistance;
								clickedGO = intersected[i];
							}
						}
						else
						{
							distance = newDistance;
							hit = true;
							clickedGO = intersected[i];
						}
					}
				}
			}

			if (hit)
			{
				//Open all the parents Imgui treenodes
				GameObject* parent = clickedGO->parent;
				while (parent)
				{
					parent->treeOpened = true;
					parent = parent->parent;
				}
			
				App->scene->selectGO(clickedGO);
			}
		}
}

void ModuleCamera3D::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::PLAY:
		{
			if (gameCamComponent && gameCamera)
			{
				activeCamComponent = gameCamComponent;
				activeCamera = gameCamera;
				activeCamComponent->RecalculateProjectionMatrix(activeCamComponent->width, activeCamComponent->height);
			}
			else
			{
				Debug.LogError("No cameras attached as MainCamera.");
				activeCamComponent = nullptr;
				activeCamera = nullptr;
			}
			break;
		}
		case EventType::STOP:
		{
			activeCamera = editorCamera;
			activeCamComponent = editorCamComponent;
			activeCamComponent->RecalculateProjectionMatrix(activeCamComponent->width, activeCamComponent->height);
			break;
		}


		case EventType::GO_DESTROYED:
		{
			if (event.goEvent.gameObject == gameCamera)
				setGameCamera(nullptr);
			break;
		}
	}
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
float4x4 ModuleCamera3D::GetViewMatrix()
{
	if (activeCamComponent)
	{
		float4x4 viewMatrix = activeCamComponent->getViewMatrix();
		return viewMatrix;
	}	
	else
		return float4x4::nan;
}

float4x4 ModuleCamera3D::GetProjMatrix()
{
	if (activeCamComponent)
	{
		float4x4 projMatrix = activeCamComponent->getProjMatrix();
		return projMatrix;
	}		
	else
		return float4x4::nan;
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
	if (activeCamComponent)
	{
		glViewport(0, 0, w, h);
		activeCamComponent->RecalculateProjectionMatrix(w, h);
	}
		
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

float4 ModuleCamera3D::getBackground() const
{
	if (activeCamComponent)
	{
		ImVec4 color = activeCamComponent->backgroundColor;
		return float4(color.x, color.y, color.z, color.w);
	}
	else
	{
		return float4::zero;
	}
}
