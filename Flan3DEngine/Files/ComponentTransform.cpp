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

