#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "imgui/imgui_stl.h"
#include "imgui/imgui_internal.h"
#include "ComponentMesh.h"

GameObject::~GameObject()
{
	destroyAABBbuffers();
	ClearChilds();
	ClearComponents();
}

bool GameObject::Update(float dt)
{
	transformAABB();

	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->Update(dt);
	}
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->Update(dt);
	}
	return true;
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
		case ComponentType::CAMERA:
		{
			ComponentCamera* cameraComp = new ComponentCamera(this);
			AddComponent(cameraComp);
			ret = cameraComp;
			break;
		}
		case ComponentType::TRANSFORM:
		{
			if (!transform)
			{
				ComponentTransform* transform = new ComponentTransform(this);
				AddComponent(transform);
				this->transform = transform;
				ret = transform;
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
	if (component->type != ComponentType::TRANSFORM || !transform) //Max 1 ComponentTransform
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
			default:
			{
				delete components[0];
				ClearComponent(components[0]);				
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

void GameObject::deleteSelected()
{
	if (selected)
	{
		//Rootnode can never be selected
	}
	else
	{
		for (int i = 0; i < childs.size() && !selected; ++i)
		{
			if (childs[i]->selected)
			{
				ComponentCamera* camcomp = (ComponentCamera*)childs[i]->getComponentByType(ComponentType::CAMERA);
				if (camcomp && camcomp->isMainCamera)
				{
					App->camera->setGameCamera(nullptr);
				}
					
				delete childs[i];
				childs.erase(childs.begin() + i);
			}
			else
			{
				childs[i]->deleteSelected();
			}
		}
	}
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

	float posY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(posY + 3);
	ImGui::Text("Name: "); ImGui::SameLine();
	ImGui::SetCursorPosY(posY);
	
	std::string tempStr = name;
	GameObject* tempPtr = (GameObject*)this;
	bool ret = ImGui::InputText("", &tempStr, flags, OnInputCallback, &tempPtr);
	if (ImGui::IsItemHovered() && !ImGui::IsItemEdited())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Press enter to apply changes");
		ImGui::EndTooltip();
	}

	ImGui::SameLine(); ImGui::Text("active"); ImGui::SameLine(); ImGui::Checkbox("##ACTIVE", &active);

	ImGui::NewLine();

	posY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(posY + 3);
	ImGui::Text("DrawAABBs"); ImGui::SameLine();
	ImGui::SetCursorPosY(posY);
	ImGui::Checkbox("##DrawAABBs", &drawAABBs);

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

	//-----------------------------Components----------------------------

	//for (int i = 0; i < components.size(); i++)
	//{
	//	names[i] = (char*)components[i]->type;
	//	toggles[i] = false;
	//}

	// Showing a menu with toggles
	if (ImGui::Button("Add/Clear Component:"))
		ImGui::OpenPopup("toggle");
	if (ImGui::BeginPopup("toggle"))
	{
		for (int i = 0; i < IM_ARRAYSIZE(names); i++)
		{
			if (getComponentByType((ComponentType)i) != nullptr)
			{
				toggles[i] = true;
			}
			ImGui::MenuItem(names[i], "", &toggles[i]);
		}

		if (toggles[(int)ComponentType::MESH] && getComponentByType(ComponentType::MESH) == nullptr)
		{
			CreateComponent(ComponentType::MESH);
		}
		else if(!toggles[(int)ComponentType::MESH] && getComponentByType(ComponentType::MESH) != nullptr)
		{
			deleteComponent(getComponentByType(ComponentType::MESH));
		}

		if (toggles[(int)ComponentType::CAMERA] && getComponentByType(ComponentType::CAMERA) == nullptr)
		{
			CreateComponent(ComponentType::CAMERA);
		}
		else if (!toggles[(int)ComponentType::CAMERA] && getComponentByType(ComponentType::CAMERA) != nullptr)
		{
			deleteComponent(getComponentByType(ComponentType::CAMERA));
		}

		if (toggles[(int)ComponentType::TRANSFORM] && getComponentByType(ComponentType::TRANSFORM) == nullptr)
		{
			ComponentMesh* Transform = new ComponentMesh(parent, true); //When we delete this?
			AddComponent(Transform);
		}

		//Put when we have materials
		//if (toggles[(int)ComponentType::MATERIAL] && getComponentByType(ComponentType::MATERIAL) == nullptr)
		//{
		//	ComponentCamera* Material = new Componentmaterial(parent, true); //When we delete this?
		//	AddComponent(Material);
		//}
		ImGui::EndPopup();
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

void GameObject::Decompose(std::vector<GameObject*>& gameObjects, std::vector<ComponentTransform*>&transforms, std::vector<ComponentMesh*>&meshes, std::vector<ComponentCamera*>&cameras)
{
	gameObjects.push_back(this);
	transforms.push_back(transform);

	ComponentMesh* mesh = (ComponentMesh*)getComponentByType(ComponentType::MESH);
	if (mesh)
		meshes.push_back(mesh);

	ComponentCamera* camera = (ComponentCamera*)getComponentByType(ComponentType::CAMERA);
	if (camera)
		cameras.push_back(camera);

	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->Decompose(gameObjects, transforms, meshes, cameras);
	}
}

void GameObject::Serialize(char*& cursor)
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &UUID, bytes);
	cursor += bytes;
	
	uint32_t parentUUID = parent ? parent->UUID : 0;
	memcpy(cursor, &parentUUID, bytes);
	cursor += bytes;

	/*if (name.length() > 50)
		Debug.LogWarning("Care: You are serializing a GameObject's name larger that 50 chars, there will be some info missing");*/

	uint nameLenght = name.length();
	bytes = sizeof(uint);
	memcpy(cursor, &nameLenght, bytes);
	cursor += bytes;

	bytes = nameLenght;
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;
}

void GameObject::DeSerialize(char*& cursor, uint32_t& parentUUID)
{
	uint bytes = sizeof(uint32_t);
	memcpy(&UUID, cursor, bytes);
	cursor += bytes;

	memcpy(&parentUUID, cursor, bytes);
	cursor += bytes;
	
	uint nameLenght = 0u;
	bytes = sizeof(uint);
	memcpy(&nameLenght, cursor, bytes);
	cursor += bytes;
	
	bytes = nameLenght;
	name.assign(cursor, cursor + bytes);
	cursor += bytes;
}

void GameObject::ReorderComponents()
{
	uint numIterations = 0;
	do
	{
		numIterations = 0;
		for (int i = 0; i < components.size(); ++i)
		{
			Component* nextComponent = i + 1 < components.size() ? components[i + 1] : nullptr;
			Component* currentComponent = components[i];

			if(nextComponent && (int)currentComponent->type > (int)nextComponent->type)
			{
				numIterations++;
				ClearComponent(currentComponent);
				InsertComponent(currentComponent, i + 1);
			}
		}
	} while (numIterations > 0);

	int a = 9;
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

void GameObject::deleteComponent(Component * component)
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] == component)
		{
			switch (component->type)
			{
			case ComponentType::MESH:
				App->renderer3D->ClearMesh((ComponentMesh*)component);
				components.erase(components.begin() + i);
				break;
			default:
				delete component;
				components.erase(components.begin() + i);
				break;
			}
			break;
		}
	}
}

int OnInputCallback(ImGuiInputTextCallbackData* callback)
{
	if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter)))
	{
		if (callback->BufTextLen == 0)
		{
			callback->InsertChars(0, "default");
		}
		GameObject* go = *(GameObject**)callback->UserData;
		go->name = callback->Buf;
	}
	
	return 0;
}

void GameObject::InsertComponent(Component* component, int pos)
{
	components.insert(components.begin() + pos, component);
}

void GameObject::drawAABB() const
{
	if (AABBvertex == nullptr || bufferIndex == 0 || !boundingBox.IsFinite())
		return;

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, numAABBvertex);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void GameObject::updateAABBbuffers()
{
	if (!boundingBox.IsFinite())
	{
		destroyAABBbuffers();
		return;
	}
		

	if (AABBvertex == nullptr)
	{
		numAABBvertex = boundingBox.NumVerticesInTriangulation(1, 1, 1);

		AABBvertex = new float[numAABBvertex * 3];

		boundingBox.Triangulate(1, 1, 1, (float3*)AABBvertex, nullptr, nullptr, true);

		glGenBuffers(1, &bufferIndex);
		glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numAABBvertex * 3, AABBvertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	numAABBvertex = boundingBox.NumVerticesInTriangulation(1,1,1);

	boundingBox.Triangulate(1, 1, 1, (float3*)AABBvertex, nullptr, nullptr, true);

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

void GameObject::recursiveDebugDraw() const
{
	debugDraw();
	for (uint i = 0; i < childs.size(); ++i)
	{
		if (this != App->scene->getRootNode())
		{
			if (!drawAABBs)
				childs[i]->drawAABBs = false;
			else
				childs[i]->drawAABBs = true;
		}
	
		childs[i]->recursiveDebugDraw();
	}
}

void GameObject::debugDraw() const
{
	if(drawAABBs && this != App->scene->getRootNode())
		drawAABB();

	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->debugDraw();
	}
}

void GameObject::transformAABB()
{
	ComponentMesh* Mesh = (ComponentMesh*)getComponentByType(ComponentType::MESH);

	boundingBox.SetNegativeInfinity();

	if (Mesh)
	{
		Mesh->updateGameObjectAABB();	
		OBB obb(boundingBox);
		obb.Transform(this->transform->getMatrix().Transposed());
		boundingBox = obb.MinimalEnclosingAABB();
		updateAABBbuffers();
	}
	else
	{
		if (parent)
		{
			Sphere sp;
			sp.pos = { 0,0,0 };
			sp.r = 0.2;
			boundingBox.Enclose(sp);

			OBB obb(boundingBox);
			ComponentTransform global = transform->getGlobal();
			obb.Transform(ComponentTransform::composeMatrix(global.position, Quat(0, 0, 0, 1), float3(1, 1, 1)).Transposed());
			boundingBox = obb.MinimalEnclosingAABB();
			updateAABBbuffers();
		}		
	}
}

void GameObject::encloseParentAABB()
{
	for (uint i = 0; i < childs.size(); ++i)
	{
		childs[i]->encloseParentAABB();
	}

	if (parent)
	{		
		parent->boundingBox.Enclose(boundingBox);
		parent->updateAABBbuffers();
	}
}
