#include "ComponentTransform.h"
#include "GameObject.h"

ComponentTransform ComponentTransform::getGlobal() const
{	
	float4x4 globalMatrix = getGlobalMatrix();
	globalMatrix = globalMatrix.Transposed();

	ComponentTransform globalTransform;
	globalMatrix.Decompose(globalTransform.position, globalTransform.rotation, globalTransform.scale);

	return globalTransform;
}

ComponentTransform ComponentTransform::getLocalWithParent(ComponentTransform* newParent) const
{
	float4x4 globalMatrixParent = newParent->getGlobalMatrix();
	globalMatrixParent = globalMatrixParent.Transposed();

	float4x4 globalMatrixChild = getGlobalMatrix();
	globalMatrixChild = globalMatrixChild.Transposed();

	float4x4 parentInverted = globalMatrixParent.Inverted();

	float4x4 newLocalMatrix = float4x4::identity;
	newLocalMatrix = parentInverted * globalMatrixChild;

	ComponentTransform local;
	newLocalMatrix.Decompose(local.position, local.rotation, local.scale);

	return local;
}

void ComponentTransform::setLocalWithParent(ComponentTransform* newParent)
{
	ComponentTransform local = getLocalWithParent(newParent);
	position = local.position;
	rotation = local.rotation;
	scale = local.scale;
}

void ComponentTransform::setLocal(ComponentTransform* newTransform)
{
	position = newTransform->position;
	rotation = newTransform->rotation;
	scale = newTransform->scale;
}

void ComponentTransform::OnInspector()
{
	float PosX = ImGui::GetCursorPosX();
	bool opened = ImGui::CollapsingHeader("##Transform"); ImGui::SameLine();

	ImGuiDragDropFlags flags = 0;
	flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
	if(ImGui::BeginDragDropSource(flags))
	{
		ImGui::BeginTooltip();
		ImGui::Text("Transform");
		ImGui::EndTooltip();
		ComponentTransform* thisOne = (ComponentTransform*)this;
		ImGui::SetDragDropPayload("DraggingComponents", &thisOne, sizeof(ComponentTransform));
		ImGui::EndDragDropSource();
	}

	ImGui::SetCursorPosX(PosX + 20);
	ImGui::Text("Transform:");	
	if(opened)
	{
		ImGui::NewLine();

		float posY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(posY + 3);
		ImGui::Text("Position: "); ImGui::SameLine();
		ImGui::SetCursorPosY(posY);
		ImGui::DragFloat3("##1", position.ptr(), .1f, -10000, 10000, "%.2f");

		posY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(posY + 3);
		ImGui::Text("Rotation: "); ImGui::SameLine();
		ImGui::SetCursorPosY(posY);
		float3 axis;
		float angleRadians;
		rotation.ToAxisAngle(axis, angleRadians);
		float angleDegrees = RadToDeg(angleRadians);
		float3 axisbyAngle = angleDegrees * axis;
		if (ImGui::DragFloat3("##2", axisbyAngle.ptr(), .1f, -180, 180, "%.2f"))
		{
			angleDegrees = axisbyAngle.Length();
			angleRadians = DegToRad(angleDegrees);
			axis = axisbyAngle.Normalized();
			rotation.SetFromAxisAngle(axis, angleRadians);

			gameObject->transformAABB();
		}

		posY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(posY + 3);
		ImGui::Text("Scale: "); ImGui::SameLine();
		ImGui::SetCursorPosY(posY);
		float posX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(posX + 21);

		float3 tempScale;
		ImGui::DragFloat3("##3", scale.ptr(), .1f, -10000, 10000, "%.2f");

		ImGui::SetCursorPosX(posX);
	}
}

float4x4 ComponentTransform::getGlobalMatrix()const
{
	if (!gameObject->parent)
	{
		return getLocalMatrix();
	}
	float4x4 globalMatrix;

	float4x4 myLocalMatrix = getLocalMatrix();
	myLocalMatrix = myLocalMatrix.Transposed();

	float4x4 myParentGlobalMatrix = gameObject->parent->transform->getGlobalMatrix();
	myParentGlobalMatrix = myParentGlobalMatrix.Transposed();

	globalMatrix = myParentGlobalMatrix * myLocalMatrix;

	return globalMatrix.Transposed();
}

float4x4 ComponentTransform::getLocalMatrix() const
{
	float4x4 ret;

	ret = float4x4::FromTRS(position, rotation, scale);

	return ret.Transposed();
}

void ComponentTransform::setFromLocalMatrix(float4x4 localMatrix)
{
	localMatrix.Transpose();
	localMatrix.Decompose(position, rotation, scale);
}

float4x4 ComponentTransform::composeMatrix(float3& position, Quat& rotation, float3& scale)
{
	float4x4 ret = float4x4::FromTRS(position, rotation, scale);
	return ret.Transposed();
}

void ComponentTransform::setFromGlobalMatrix(float4x4 matrix)
{	
	float4x4 localMatrix = float4x4::identity;
	if (gameObject->parent)
	{
		float4x4 parentGlobalMatrix = gameObject->parent->transform->getGlobalMatrix();
		parentGlobalMatrix = parentGlobalMatrix.Transposed();
		parentGlobalMatrix = parentGlobalMatrix.Inverted();
		localMatrix = parentGlobalMatrix * matrix;
	}
	else
	{
		localMatrix = matrix;
	}

	localMatrix.Decompose(position, rotation, scale);
}

void ComponentTransform::Serialize(char*& cursor) const
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &gameObject->uuid, bytes);
	cursor += bytes;

	bytes = sizeof(float3);
	memcpy(cursor, position.ptr(), bytes);
	cursor += bytes;
	bytes = sizeof(Quat);
	memcpy(cursor, rotation.ptr(), bytes);
	cursor += bytes;
	bytes = sizeof(float3);
	memcpy(cursor, scale.ptr(), bytes);
	cursor += bytes;
}

void ComponentTransform::DeSerialize(char *& cursor, uint32_t & goUUID)
{
	uint bytes = sizeof(uint32_t);
	memcpy(&goUUID, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float3);
	memcpy(position.ptr(), cursor, bytes);
	cursor += bytes;

	bytes = sizeof(Quat);
	memcpy(rotation.ptr(), cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float3);
	memcpy(scale.ptr(), cursor, bytes);
	cursor += bytes;
}

