#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "imgui/imgui_stl.h"
#include "imgui/imgui_internal.h"

GameObject::~GameObject()
{
	destroyAABBbuffers();
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
			ret = (Component*)App->renderer3D->CreateComponentMesh(this);
			AddComponent(ret);
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
			
	if (components.size() > 0)
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::Dummy({ ImGui::GetWindowWidth(), 5 });
		ImGui::SetCursorScreenPos(cursorPos);
		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags flags = 0;
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingComponents", flags);
			if (payload)
			{
				compToReorder = *(Component**)payload->Data;
				if (compToReorder != components.front())
				{
					//Draw a line
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					ImGui::ItemSize(ImVec2(0.0f, 0.0f));
					float x1 = window->Pos.x;
					float x2 = window->Pos.x + window->Size.x;
					ImRect rec{ ImVec2(x1, window->DC.CursorPos.y-2), ImVec2(x2, window->DC.CursorPos.y-2)};
					window->DrawList->AddLine(rec.Min, ImVec2(rec.Max.x, rec.Min.y), ImGui::GetColorU32(ImGuiCol_::ImGuiCol_DragDropTarget), 2);

					if (ImGui::IsMouseReleased(0))
					{
						postoreorder = 0;
					}
					else
					{
						compToReorder = nullptr;

					}
				}
				else
				{
					compToReorder = nullptr;
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Separator();
	}

	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnInspector();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::Dummy({ ImGui::GetWindowWidth(), 5 });
		ImGui::SetCursorScreenPos(cursorPos);
		if (ImGui::BeginDragDropTarget())
		{
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingComponents", flags);
			if (payload)
			{
				compToReorder = *(Component**)payload->Data;
				if ( components[i] != compToReorder && (i < components.size()-1 ? (components[i+1] != compToReorder) : 1))
				{
					//Draw a line
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					ImGui::ItemSize(ImVec2(0.0f, 0.0f));
					float x1 = window->Pos.x;
					float x2 = window->Pos.x + window->Size.x;
					ImRect rec{ ImVec2(x1, window->DC.CursorPos.y - 2), ImVec2(x2, window->DC.CursorPos.y - 2) };
					window->DrawList->AddLine(rec.Min, ImVec2(rec.Max.x, rec.Min.y), ImGui::GetColorU32(ImGuiCol_::ImGuiCol_DragDropTarget), 2);
					
					if(ImGui::IsMouseReleased(0))
					{
						postoreorder = i;
					}
					else
					{
						compToReorder = nullptr;
						
					}
				}
				else
				{
					compToReorder = nullptr;
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Separator();

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

void GameObject::drawAABB() const
{
	if (AABBvertex == nullptr || bufferIndex == 0)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, numAABBvertex);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void GameObject::createAABBbuffers()
{
	if (!boundingBox.IsFinite())
		return;

	numAABBvertex = boundingBox.NumVerticesInTriangulation(1,1,1);

	AABBvertex = new float[numAABBvertex * 3];

	boundingBox.Triangulate(1, 1, 1, (float3*)AABBvertex, nullptr, nullptr, true);

	glGenBuffers(1, &bufferIndex);
	glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numAABBvertex * 3, AABBvertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void GameObject::destroyAABBbuffers()
{
	if (AABBvertex == nullptr || bufferIndex == 0)
		return;

	delete AABBvertex;
	AABBvertex = nullptr;

	boundingBox.SetNegativeInfinity();

	glDeleteBuffers(1, &bufferIndex);
}

void GameObject::recursiveDebugDraw(GameObject* gameObject) const
{
	debugDraw();
	for (uint i = 0; i < gameObject->childs.size(); ++i)
	{
		recursiveDebugDraw(childs[i]);
	}
}

void GameObject::debugDraw() const
{
	drawAABB();
}
