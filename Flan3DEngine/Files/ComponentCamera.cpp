#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ComponentCamera::ComponentCamera(GameObject* gameObject, bool active) : Component(ComponentType::CAMERA, gameObject, active)
{
	if (!gameObject)
		Debug.LogWarning("No gameObject attached to this camera component");

	width = height = 50.0f;
	aspectRatio = width / height;
	horizontalFOV = DegToRad(60);
	verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));

	nearDistance = 0.125f;
	farDistance = 10.0f;//1000.0f;

	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = verticalFOV;
}

ComponentCamera::~ComponentCamera()
{
	if (vertexID != 0)
	{
		glDeleteBuffers(1, &vertexID);
		vertexID = 0;
		delete vertex;
		vertex = nullptr;
	}

	if (indexID != 0)
	{
		glDeleteBuffers(1, &indexID);
		indexID = 0;
	}
}

bool ComponentCamera::Update(float dt)
{
	if (!vertex || !index)
	{
		glGenBuffers(1, (GLuint*)&vertexID);
		vertex = new float[8 * 3];

		glGenBuffers(1, &indexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 24, index, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	ComponentTransform global = gameObject->transform->getGlobal();

	frustum.front = global.rotation * float3(0, 0, 1);
	frustum.up = global.rotation * float3(0, 1, 0);
	frustum.pos = global.position;

	float3 cornerPoints[8];
	frustum.GetCornerPoints(cornerPoints);

	memcpy(vertex, cornerPoints, sizeof(float) * 8 * 3);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

void ComponentCamera::RecalculateProjectionMatrix(int w, int h)
{
	this->aspectRatio = w / h;
	width = w; 
	height = h;
	verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));

	updateFrustum();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float4x4 mathGeoProjMatrix = frustum.ProjectionMatrix();
	float4x4 fixedOpenGLProjMatrix = float4x4::identity;

	fixedOpenGLProjMatrix.SetCol(0, mathGeoProjMatrix.Row(0));
	fixedOpenGLProjMatrix.SetCol(1, mathGeoProjMatrix.Row(1));
	fixedOpenGLProjMatrix.SetCol(2, mathGeoProjMatrix.Row(2));
	fixedOpenGLProjMatrix.SetCol(3, mathGeoProjMatrix.Row(3));

	glLoadMatrixf(mathGeoProjMatrix.Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float4x4 ComponentCamera::getViewMatrix()
{
	float4x4 mathGeoViewMatrix = frustum.ViewMatrix();
	return mathGeoViewMatrix.Transposed();
}

float4x4 ComponentCamera::getProjMatrix()
{
	float4x4 mathGeoProjMatrix = frustum.ProjectionMatrix();
	return mathGeoProjMatrix.Transposed();
}

void ComponentCamera::updateFrustum()
{
	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = verticalFOV;
}

void ComponentCamera::calculateVerticalFOV()
{
	verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));
}

void ComponentCamera::Serialize(char*& cursor) const
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &gameObject->uuid, bytes);
	cursor += bytes;

	bytes = sizeof(ImVec4);
	memcpy(cursor, &backgroundColor, bytes);
	cursor += bytes;

	bytes = sizeof(float);
	memcpy(cursor, &nearDistance, bytes);
	cursor += bytes;

	memcpy(cursor, &farDistance, bytes);
	cursor += bytes;

	memcpy(cursor, &aspectRatio, bytes);
	cursor += bytes;

	memcpy(cursor, &width, bytes);
	cursor += bytes;

	memcpy(cursor, &height, bytes);
	cursor += bytes;

	memcpy(cursor, &horizontalFOV, bytes);
	cursor += bytes;

	memcpy(cursor, &verticalFOV, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(cursor, &isMainCamera, bytes);
	cursor += bytes;
}

void ComponentCamera::DeSerialize(char*& cursor, uint32_t& goUUID)
{
	uint bytes = sizeof(uint32_t);
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(ImVec4);
	memcpy(&backgroundColor, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float);
	memcpy(&nearDistance, cursor, bytes);
	cursor += bytes;

	memcpy(&farDistance, cursor, bytes);
	cursor += bytes;

	memcpy(&aspectRatio, cursor, bytes);
	cursor += bytes;

	memcpy(&width, cursor, bytes);
	cursor += bytes;

	memcpy(&height, cursor, bytes);
	cursor += bytes;

	memcpy(&horizontalFOV, cursor, bytes);
	cursor += bytes;

	memcpy(&verticalFOV, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(bool);
	memcpy(&isMainCamera, cursor, bytes);
	cursor += bytes;
}

void ComponentCamera::setMainCamera()
{	
	isMainCamera = true;
	App->camera->setGameCamera(this);
}

void ComponentCamera::debugDraw()
{
	if (this == App->camera->editorCamComponent || vertexID == 0)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ComponentCamera::OnInspector()
{
	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader("##Camera"); ImGui::SameLine();

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("RightClickCameraMenu");
	}

	ImGui::SetNextWindowSize({ 150, 45 });

	ImGuiWindowFlags wflags = 0;
	wflags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

	if (ImGui::BeginPopup("RightClickCameraMenu", wflags))
	{
		if (ImGui::MenuItem("Delete Component"))
		{
			Event event;
			event.type = EventType::COMPONENT_DESTROYED;
			event.compEvent.component = this;
			App->SendEvent(event);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGuiDragDropFlags flags = 0;
	flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
	if (ImGui::BeginDragDropSource(flags))
	{
		ImGui::BeginTooltip();
		ImGui::Text("Camera");
		ImGui::EndTooltip();
		ComponentTransform* thisOne = (ComponentTransform*)this;
		ImGui::SetDragDropPayload("DraggingComponents", &thisOne, sizeof(ComponentTransform));
		ImGui::EndDragDropSource();
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text("Camera:");

	if (opened)
	{
		//TODO: Move tags to the left of the values, order all the widgets
		static float wh[2] = { width,height };
		if (ImGui::DragFloat2("Size", wh, 1, 10, 1000, "%.0f"))
		{		
			width = wh[0];
			height = wh[1];
			verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));
			aspectRatio = width / height;
			updateFrustum();
			if (this == App->camera->activeCamComponent)
			{
				RecalculateProjectionMatrix(width, height);
			}
		}
		
		float hFOVDegrees = RadToDeg(horizontalFOV);
		if (ImGui::DragFloat("FOV", &hFOVDegrees, .1, 10, 90, "%.1f"))
		{
			horizontalFOV = DegToRad(hFOVDegrees);
			verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));
			updateFrustum();
			if (this == App->camera->activeCamComponent)
			{
				RecalculateProjectionMatrix(width, height);
			}
		}

		if (ImGui::DragFloat("Far Distance", &farDistance, .1, 10, 2000, "%.2f"))
		{
			updateFrustum();
			if (this == App->camera->activeCamComponent)
			{
				RecalculateProjectionMatrix(width, height);
			}
		}

		if (ImGui::Checkbox("Main Camera", &isMainCamera))
		{
			if (isMainCamera == true)
			{
				App->camera->setGameCamera(this);
			}
			else
			{
				App->camera->setGameCamera(nullptr);
			}
		}

		ImGui::ColorEdit3("Background Color", &backgroundColor.x, 0);
	}
}
