#include "ComponentCamera.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* parent, bool active) : Component(ComponentType::CAMERA, parent, active)
{
	if (!parent)
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

	frustum.front = gameObject->transform->rotation * float3(0, 0, 1);
	frustum.up = gameObject->transform->rotation * float3(0, 1, 0);
	frustum.pos = gameObject->transform->position;

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
	glViewport(0, 0, w, h);
	this->aspectRatio = w / h;
	width = w; 
	height = h;
	verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));

	//Update frustum
	frustum.verticalFov = verticalFOV;
	frustum.farPlaneDistance = farDistance;
	frustum.nearPlaneDistance = nearDistance;

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
	float4x4 fixedOpenGLViewMatrix = float4x4::identity;
	
	fixedOpenGLViewMatrix.SetCol(0, mathGeoViewMatrix.Row(0));
	fixedOpenGLViewMatrix.SetCol(1, mathGeoViewMatrix.Row(1));
	fixedOpenGLViewMatrix.SetCol(2, mathGeoViewMatrix.Row(2));
	fixedOpenGLViewMatrix.SetCol(3, mathGeoViewMatrix.Row(3));

	return mathGeoViewMatrix.Transposed();
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

void ComponentCamera::debugDraw()
{
	if (this == App->camera->editorCamComponent)
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
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, { .2f,.2f,.9f,.5f });
	bool opened = ImGui::CollapsingHeader("##Camera"); ImGui::SameLine();

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

	ImGui::PopStyleColor();
	ImGui::SetCursorPosX(PosX + 20);
	ImGui::TextColored({ 1,1,0,1 }, "Camera:");

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
		}
		
		float hFOVDegrees = RadToDeg(horizontalFOV);
		if (ImGui::DragFloat("FOV", &hFOVDegrees, .1, 10, 90, "%.1f"))
		{
			horizontalFOV = DegToRad(hFOVDegrees);
			verticalFOV = 2 * atanf(tanf(horizontalFOV / 2) * (height / width));
			updateFrustum();
		}

		if (ImGui::DragFloat("Far Distance", &farDistance, .1, 10, 2000, "%.2f"))
		{
			updateFrustum();
		}

		if (ImGui::Checkbox("Main Camera", &isMainCamera))
		{
			//TODO: Do some stuff in render to keep the game view with this camera
		}

		if (ImGui::ColorEdit3("Background Color", &backgroundColor.x, 0))
		{
			//TODO: CHANGE THE BACKGROUND
		}
	}
}
