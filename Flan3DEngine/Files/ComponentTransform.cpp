#include "ComponentTransform.h"
#include "GameObject.h"

ComponentTransform ComponentTransform::getGlobal() const
{	
	std::list<float4x4> temp;

	GameObject* currentGO = this->gameObject;
	while(currentGO)
	{
		temp.push_back(currentGO->transform->getLocalMatrix().Transposed());
		currentGO = currentGO->parent;
	}
	
	float4x4 globalMatrix = float4x4::identity;
	for (std::list<float4x4>::reverse_iterator it = temp.rbegin(); it != temp.rend(); ++it)
	{
		globalMatrix = globalMatrix * (*it);
	}

	ComponentTransform globalTransform;
	globalMatrix.Decompose(globalTransform.position, globalTransform.rotation, globalTransform.scale);

	return globalTransform;
}

ComponentTransform ComponentTransform::getLocal(ComponentTransform* newParent) const
{
	ComponentTransform local;

	ComponentTransform childGlobal = getGlobal();

	ComponentTransform parentGlobal = newParent->getGlobal();

	local.position = childGlobal.position - parentGlobal.position;
	local.rotation = childGlobal.rotation / parentGlobal.rotation;
	local.scale = childGlobal.scale.Div(parentGlobal.scale);

	return local;
}

void ComponentTransform::setLocalWithParent(ComponentTransform* newParent)
{
	ComponentTransform local = getLocal(newParent);
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

void ComponentTransform::setLocalWithParentGlobal(ComponentTransform parentGlobal)
{
	ComponentTransform childGlobal = getGlobal();

	position = childGlobal.position - parentGlobal.position;
	rotation = childGlobal.rotation / parentGlobal.rotation;
	scale = childGlobal.scale.Div(parentGlobal.scale);
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
		}

		posY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(posY + 3);
		ImGui::Text("Scale: "); ImGui::SameLine();
		ImGui::SetCursorPosY(posY);
		float posX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(posX + 21);

		float3 tempScale;
		ImGui::DragFloat3("##3", scale.ptr(), .1f, -10000, 10000, "%.2f");

		//scale = {Equal(tempScale.x, 0) ? 0.1f : tempScale.x, Equal(tempScale.y, 0) ? 0.1f : tempScale.y, Equal(tempScale.z, 0) ? 0.1f : tempScale.z};

		ImGui::SetCursorPosX(posX);
	}
}

float4x4 ComponentTransform::getGlobalMatrix()const
{
	float4x4 ret;

	ComponentTransform global = getGlobal();
	ret = float4x4::FromTRS(global.position, global.rotation, global.scale);

	return ret.Transposed();
}

float4x4 ComponentTransform::getLocalMatrix() const
{
	float4x4 ret;

	ret = float4x4::FromTRS(position, rotation, scale);

	return ret.Transposed();
}

float4x4 ComponentTransform::composeMatrix(float3& position, Quat& rotation, float3& scale)
{
	return float4x4::FromTRS(position, rotation, scale).Transposed();
}

void ComponentTransform::setFromGlobalMatrix(float4x4 matrix)
{	
	float4x4 localMatrix = float4x4::identity;
	if (gameObject->parent)
	{
		float4x4 parentGlobalMatrix = gameObject->transform->getGlobalMatrix().Transposed();
		localMatrix = parentGlobalMatrix.Inverted() * matrix;
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

