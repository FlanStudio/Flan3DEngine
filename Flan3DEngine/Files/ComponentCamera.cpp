#pragma once

#include "ComponentCamera.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* parent, bool active) : Component(ComponentType::CAMERA, parent, active)
{
	if (!parent)
		Debug.LogWarning("No gameObject attached to this camera component");

	width = height = 50.0f;
	aspectRatio = width / height;
	verticalFOV = 100;
	horizontalFOV = RadToDeg(2 * Atan(tan(DegToRad(verticalFOV / 2)) * (width / height)));

	nearDistance = 0.125f;
	farDistance = 10.0f;//1000.0f;

	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.horizontalFov = DegToRad(horizontalFOV);
	frustum.verticalFov = DegToRad(verticalFOV);
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

void ComponentCamera::RecalculateProjectionMatrix(int w, int h)
{
	glViewport(0, 0, w, h);
	this->aspectRatio = w / h;
	width = w; 
	height = h;
	horizontalFOV = RadToDeg(2 * Atan(tan(DegToRad(verticalFOV / 2)) * (width / height)));

	//Update frustum
	frustum.horizontalFov = DegToRad(horizontalFOV);
	frustum.farPlaneDistance = farDistance;
	frustum.nearPlaneDistance = nearDistance;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float4x4 projMatrix;

	float coty = 1.0f / tan(verticalFOV * (float)pi / 360.0f);
	projMatrix[0][0] = coty / aspectRatio;
	projMatrix[0][1] = 0.0f;
	projMatrix[0][2] = 0.0f;
	projMatrix[0][3] = 0.0f;
	projMatrix[1][0] = 0.0f;
	projMatrix[1][1] = coty;
	projMatrix[1][2] = 0.0f;
	projMatrix[1][3] = 0.0f;
	projMatrix[2][0] = 0.0f;
	projMatrix[2][1] = 0.0f;
	projMatrix[2][2] = (nearDistance + farDistance) / (nearDistance - farDistance);
	projMatrix[2][3] = -1.0f;
	projMatrix[3][0] = 0.0f;
	projMatrix[3][1] = 0.0f;
	projMatrix[3][2] = 2.0f * nearDistance * farDistance / (nearDistance - farDistance);
	projMatrix[3][3] = 0.0f;

	glLoadMatrixf(projMatrix.ptr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

float4x4 ComponentCamera::getViewMatrix()
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

	//there is no update at the moment so we have to update the frustum transform here
	frustum.front = gameObject->transform->rotation * float3(0,0,1);
	frustum.up = gameObject->transform->rotation * float3(0, 1, 0);	
	frustum.pos = gameObject->transform->position;

	float3 cornerPoints[8];
	frustum.GetCornerPoints(cornerPoints);
	
	memcpy(vertex, cornerPoints, sizeof(float) * 8 * 3);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	debugDraw();

	float4x4 mathGeoViewMatrix = frustum.ViewMatrix();
	float4x4 fixedOpenGLViewMatrix = float4x4::identity;
	
	fixedOpenGLViewMatrix.SetCol(0, mathGeoViewMatrix.Row(0));
	fixedOpenGLViewMatrix.SetCol(1, mathGeoViewMatrix.Row(1));
	fixedOpenGLViewMatrix.SetCol(2, mathGeoViewMatrix.Row(2));
	fixedOpenGLViewMatrix.SetCol(3, mathGeoViewMatrix.Row(3));

	return fixedOpenGLViewMatrix;
}

void ComponentCamera::updateFrustum()
{
	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.horizontalFov = DegToRad(horizontalFOV);
	frustum.verticalFov = DegToRad(verticalFOV);
}

void ComponentCamera::calculateHorizontalFOV()
{
	horizontalFOV = RadToDeg(2 * Atan(tan(DegToRad(verticalFOV / 2)) * (width / height)));
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
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, { .2,.2,.9,.5 });
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
		static float wh[2] = { 50,50 };
		if (ImGui::DragFloat2("Size", wh, 1, 10, 1000, "%.0f"))
		{		
			width = wh[0];
			height = wh[1];
			horizontalFOV = RadToDeg(2 * Atan(tan(DegToRad(verticalFOV / 2)) * (width / height)));
			aspectRatio = width / height;
			updateFrustum();
		}
		
		if (ImGui::DragFloat("FOV", &verticalFOV, .1, 10, 90, "%.1f"))
		{
			horizontalFOV = RadToDeg(2 * Atan(tan(DegToRad(verticalFOV / 2)) * (width / height)));
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
