#include "ComponentTransform.h"
#include "GameObject.h"

ComponentTransform ComponentTransform::getGlobal() const
{
	ComponentTransform global;
	global.position = position;
	global.rotation = rotation;
	global.scale = scale;

	GameObject* parent = gameObject->parent;
	while (parent)
	{
		global.position += parent->transform->position;
		global.rotation = global.rotation.Mul(parent->transform->rotation);
		global.scale = global.scale.Mul(parent->transform->scale);
		parent = parent->parent;
	}
	return global;
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
		ImGui::DragFloat3("##3", scale.ptr(), .1f, -10000, 10000, "%.2f");
		ImGui::SetCursorPosX(posX);
	}
}

float4x4 ComponentTransform::getMatrix()const
{
	float4x4 ret;

	ComponentTransform global = getGlobal();
	ret = float4x4::FromTRS(global.position, global.rotation, global.scale);

	return ret.Transposed();
}

float4x4 ComponentTransform::composeMatrix(float3& position, Quat& rotation, float3& scale)
{
	return float4x4::FromTRS(position, rotation, scale).Transposed();
}

void ComponentTransform::setFromMatrix(float4x4 matrix)
{	
	matrix.Decompose(position, rotation, scale);
	if (gameObject->parent)
	{
		position = position - gameObject->parent->transform->position;
		rotation = rotation / gameObject->parent->transform->rotation;
		scale = scale.Div(gameObject->parent->transform->scale);
	}
}

void ComponentTransform::Serialize(char*& cursor) const
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &gameObject->UUID, bytes);
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

