#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "imgui/imgui_stl.h"

GameObject::~GameObject()
{
	ClearChilds();
	ClearComponents();
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* ret = nullptr;

	switch (type)
	{
		case ComponentType::MESH:
		{
			//TODO: App renderer create component mesh, and save the pointer returned into our vector
			break;
		}
		case ComponentType::TRANSFORM:
		{
			if (!hasTransform)
			{
				ComponentTransform* transform = new ComponentTransform(this);
				AddComponent(transform);
				hasTransform = true;
				this->transform = transform;
			}
			else
			{
				Debug.LogWarning("This GameObject already has a ComponentTransform!");
			}
			
			break;
		}
	}
	return ret;
}

void GameObject::AddComponent(Component* component)
{
	if (component->type != ComponentType::TRANSFORM || !hasTransform) //Max 1 ComponentTransform
	{
		components.push_back(component);
		if (component->type == ComponentType::TRANSFORM)
			this->transform = transform;
	}
		
	
}

void GameObject::AddChild(GameObject* child)
{
	childs.push_back(child);
}

void GameObject::ClearChilds()
{
	for (int i = 0; i < childs.size(); ++i)
	{
		delete childs[i];
	}
	childs.clear();
}

void GameObject::ClearComponents()
{
	while(components.size() > 0)
	{
		switch (components[0]->type)
		{
			case ComponentType::MESH:
			{
				App->renderer3D->ClearMesh((ComponentMesh*)components[0]);
				ClearComponent(components[0]);
				break;
			}
			case ComponentType::TRANSFORM:
			{
				ClearComponent(components[0]);
				delete transform;
				transform = nullptr;
				break;
			}
		}
	}
}

void GameObject::ClearComponent(Component* component)
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] == component)
		{
			components.erase(components.begin() + i);
			break;
		}
	}
}

bool GameObject::HasChilds() const
{
	return childs.size() > 0;
}

GameObject* GameObject::getSelectedGO() const
{
	GameObject* ret = nullptr;

	if (selected)
	{
		ret = (GameObject*)this;
	}
	else
	{
		for (int i = 0; i < childs.size() && !ret; ++i)
		{
			if (childs[i]->selected)
			{
				ret = childs[i];
			}
			else
			{
				ret = childs[i]->getSelectedGO();			
			}	
		}
	}

	return ret;
}

void GameObject::ClearChild(GameObject* child)
{
	for (int i = 0; i < childs.size(); ++i)
	{
		if (childs[i] == child)
		{
			childs.erase(childs.begin() + i);
			break;
		}
	}
}

void GameObject::OnInspector()
{	
	ImGuiInputTextFlags flags = 0;
	flags |= ImGuiInputTextFlags_AutoSelectAll;
	flags |= ImGuiInputTextFlags_AllowTabInput;
	flags |= ImGuiInputTextFlags_CallbackAlways;
	flags |= ImGuiInputTextFlags_CallbackCharFilter;
	flags |= ImGuiInputTextFlags_EnterReturnsTrue;
	flags |= ImGuiInputTextFlags_AlwaysInsertMode;

	float posY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(posY + 3);
	ImGui::Text("Name: "); ImGui::SameLine();
	ImGui::SetCursorPosY(posY);
	
	bool ret = ImGui::InputText("", &name, flags, OnInputCallback, &ret);

	ImGui::NewLine();
	int postoreorder = -1;
	Component* compToReorder = nullptr;
	if(components.size() > 0)
		ImGui::Separator();
	for (int i = 0; i < components.size(); ++i)
	{		
		components[i]->OnInspector();
		ImGui::Separator();
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingComponents", 0); //Set flags
			if (payload)
			{
				compToReorder = *(Component**)payload->Data;
				postoreorder = i;
			}
			ImGui::EndDragDropTarget();
		}
	}

	if (postoreorder >= 0 && compToReorder)
	{
		ClearComponent(compToReorder);
		InsertComponent(compToReorder, postoreorder);
	}

}

void GameObject::InsertChild(GameObject* child, int pos)
{
	childs.insert(childs.begin() + pos, child);
}

int GameObject::getChildPos(const GameObject* child) const
{
	for (int i = 0; i < childs.size(); ++i)
	{
		if (childs[i] == child)
			return i;
	}
}

Component* GameObject::getComponentByType(ComponentType type) const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == type)
		{
			return components[i];
		}
	}
	return nullptr;
}

int OnInputCallback(ImGuiInputTextCallbackData* callback)
{
	if (callback->BufTextLen == 0 && (bool)callback->UserData == true && ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter)))
	{
		callback->InsertChars(0, "default");
	}
	return 0;
}

void GameObject::InsertComponent(Component* component, int pos)
{
	components.insert(components.begin() + pos, component);
}
