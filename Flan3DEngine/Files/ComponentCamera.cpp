#pragma once

#include "ComponentCamera.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* parent, bool active) : Component(ComponentType::CAMERA, parent, active)
{
	if (!parent)
		Debug.LogWarning("No gameObject attached to this camera component");

	aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;

	verticalFOV = 60;
	horizontalFOV = 2 * Atan(tan(verticalFOV / 2) * (SCREEN_WIDTH / SCREEN_HEIGHT));

	nearDistance = 0.125f;
	farDistance = 1000.0f;

	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = verticalFOV;
}

void ComponentCamera::RecalculateProjectionMatrix(int w, int h)
{
	glViewport(0, 0, w, h);
	this->aspectRatio = w / h;
	SCREEN_WIDTH = w; 
	SCREEN_HEIGHT = h;
	horizontalFOV = 2 * Atan(tan(verticalFOV / 2) * (SCREEN_WIDTH / SCREEN_HEIGHT));

	//Update frustum
	frustum.horizontalFov = horizontalFOV;

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
	//there is no update at the moment so we have to update the frustum transform
	frustum.front = gameObject->transform->rotation * float3(0,0,1);
	frustum.up = gameObject->transform->rotation * float3(0, 1, 0);	
	frustum.pos = gameObject->transform->position;

	float4x4 mathGeoViewMatrix = frustum.ViewMatrix();
	float4x4 fixedOpenGLViewMatrix = float4x4::identity;
	
	fixedOpenGLViewMatrix.SetCol(0, mathGeoViewMatrix.Row(0));
	fixedOpenGLViewMatrix.SetCol(1, mathGeoViewMatrix.Row(1));
	fixedOpenGLViewMatrix.SetCol(2, mathGeoViewMatrix.Row(2));
	fixedOpenGLViewMatrix.SetCol(3, mathGeoViewMatrix.Row(3));

	return fixedOpenGLViewMatrix;
}

void ComponentCamera::OnInspector()
{
	//Check if we are changing the good camera
}
