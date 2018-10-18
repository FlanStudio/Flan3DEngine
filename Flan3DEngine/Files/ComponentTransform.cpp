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
		global.rotation.Mul(parent->transform->rotation);
		global.scale.Mul(parent->transform->scale);
		parent = parent->parent;
	}

	return global;
}

ComponentTransform ComponentTransform::getLocal(ComponentTransform parent) const
{
	ComponentTransform local;

	ComponentTransform global = getGlobal();
	local.position = global.position - position;
	local.rotation = global.rotation / rotation;
	local.scale = global.scale.Div(scale);

	return local;
}

void ComponentTransform::setLocal(ComponentTransform parent)
{
	ComponentTransform local = getLocal(parent);
	position = local.position;
	rotation = local.rotation;
	scale = local.scale;
}

void ComponentTransform::OnInspector()
{
	ImGui::TextColored({ 1,1,0,1 },"Transform:");
	
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

	float3 rotationEuler = RadToDeg(rotation.ToEulerXYZ());
	ImGui::DragFloat3("##2", rotationEuler.ptr(), .1f, -10000, 10000, "%.2f");
	rotationEuler = { DegToRad(rotationEuler.x), DegToRad(rotationEuler.y), DegToRad(rotationEuler.z) };
	rotation = rotation.FromEulerXYZ(rotationEuler.x, rotationEuler.y, rotationEuler.z);

	posY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(posY + 3);	
	ImGui::Text("Scale: "); ImGui::SameLine();
	ImGui::SetCursorPosY(posY);
	float posX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(posX + 21);
	ImGui::DragFloat3("##3", scale.ptr(), .1f, -10000, 10000, "%.2f");
	ImGui::SetCursorPosX(posX);
}