#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "Brofiler\Brofiler.h"

#include "imgui/imgui_stl.h"
#include "imgui/imgui_internal.h"


GameObject::~GameObject()
{
	Event event;
	event.goEvent.type = EventType::GO_DESTROYED;
	event.goEvent.gameObject = this;
	App->SendEvent(event);

	destroyAABBbuffers();
	ClearChilds();
	ClearComponents();
}

bool GameObject::Update(float dt)
{
	BROFILER_CATEGORY("GameObjUpdate", Profiler::Color::Azure)

	for (int i = 0; i < childs.size(); ++i)
	{
		if (childs[i]->isActive())
			childs[i]->Update(dt);
		//else
		//	for each (Component* component in childs[i]->components)
		//	{
		//		component->Disable();
		//		int j = 98;
		//	}
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
		case ComponentType::MATERIAL:
		{
			ComponentMaterial* material = new ComponentMaterial(this);
			AddComponent(material);
			ret = material;
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
		component->gameObject = this;
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

int GameObject::getComponentIndex(const Component * component) const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i] == component)
			return i;
	}
	return -1;
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

void GameObject::ClearComponentAt(int i)
{
	if (i < components.size())
		components.erase(components.begin() + i);
}

bool GameObject::HasChilds() const
{
	return childs.size() > 0;
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
	ImVec2 pos = ImGui::GetCursorPos();

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

	if (ImGui::Checkbox("##ACTIVE", &active)); ImGui::SameLine(); ImGui::Text("Active");

	//ImGui::NewLine();

	posY = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(posY + 3);
	ImGui::Checkbox("##DrawAABBs", &drawAABBs);	
	ImGui::SameLine();
	ImGui::SetCursorPosY(posY);
	ImGui::Text("DrawAABBs");

	int postoreorder = -1;
	Component* compToReorder = nullptr;
	int compToReorderIndex = -1;
	if (components.size() > 0)
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::Dummy({ ImGui::GetWindowWidth(), 5 }); //The line above the first component
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
				compToReorderIndex = getComponentIndex(compToReorder);
				if (compToReorder != components.front()) //If the one im dragging is different that the already placed in the first place
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
		ImGui::Dummy({ ImGui::GetWindowWidth(), 5 }); //The line after the rendered component, which has an index of i
		ImGui::SetCursorScreenPos(cursorPos);
		if (ImGui::BeginDragDropTarget())
		{
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
			flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingComponents", flags);
			if (payload)
			{
				compToReorder = *(Component**)payload->Data;
				compToReorderIndex = getComponentIndex(compToReorder);
				if ( components[i] != compToReorder && (compToReorderIndex != i + 1)) //If im not dropping a component to the same position and the component im dropping isn't already in place
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
						postoreorder = i+1;  //Set the desired position to the next one of the component indexed as i
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
		InsertComponent(compToReorder, postoreorder);
		ClearComponentAt(postoreorder > compToReorderIndex ? compToReorderIndex : compToReorderIndex +1);
	}

	ImVec2 normalPos = ImGui::GetCursorPos();

	ImGui::SetCursorPos({ pos.x - 15, pos.y });
	ImGui::Dummy({ ImGui::GetWindowSize().x,  ImGui::GetWindowSize().y - 40 });

	//Dropping scripts
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags flags = 0;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingCS", flags);
		if (payload)
		{
			char* path = (char*)payload->Data;
			if (path != nullptr)
			{
				//Create the script component with this cs path
				std::string scriptName(path);
				scriptName = scriptName.substr(scriptName.find_last_of("/")+1);
				scriptName = scriptName.substr(0, scriptName.find_last_of("."));

				Component* script = (Component*)App->scripting->CreateScriptComponent(scriptName, false);
				App->scene->getSelectedGO()->AddComponent(script);
				script->gameObject = App->scene->getSelectedGO();
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SetCursorPos(normalPos);
}

void GameObject::ReceiveEvent(Event event)
{
	switch (event.type)
	{
		case EventType::GO_DESTROYED:
		{
			for(int i = 0; i < childs.size(); ++i)
			{
				if (childs[i] == event.goEvent.gameObject)
				{
					childs.erase(childs.begin() + i);
					i--;
				}
				else
					childs[i]->ReceiveEvent(event);
			}
			break;
		}
		case EventType::COMPONENT_DESTROYED:
		{
			deleteComponent(event.compEvent.component);
			for (int i = 0; i < childs.size(); ++i)
			{
				childs[i]->ReceiveEvent(event);
			}
			break;
		}
	}
}

void GameObject::SetActive(bool boolean)
{
	if (active == !boolean)
	{
		active = boolean;
		//TODO: Some event throwing here if we are in playMode, in order to notify the components, specially Scripts.
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

void GameObject::Decompose(std::vector<GameObject*>& gameObjects, std::vector<ComponentTransform*>&transforms, std::vector<ComponentMesh*>&meshes, 
						   std::vector<ComponentCamera*>&cameras, std::vector<ComponentMaterial*>& materials)
{	
	transforms.push_back(transform);

	ComponentMesh* mesh = (ComponentMesh*)getComponentByType(ComponentType::MESH);
	if (mesh)
		meshes.push_back(mesh);

	ComponentCamera* camera = (ComponentCamera*)getComponentByType(ComponentType::CAMERA);
	if (camera)
		cameras.push_back(camera);

	ComponentMaterial* material = (ComponentMaterial*)getComponentByType(ComponentType::MATERIAL);
	if (material)
		materials.push_back(material);

	for (int i = 0; i < childs.size(); ++i)
	{
		gameObjects.push_back(childs[i]);
		childs[i]->Decompose(gameObjects, transforms, meshes, cameras, materials);
	}
}

void GameObject::Serialize(char*& cursor)
{
	uint bytes = sizeof(uint32_t);
	memcpy(cursor, &uuid, bytes);
	cursor += bytes;
	
	uint32_t parentUUID = parent ? parent->uuid : 0;
	memcpy(cursor, &parentUUID, bytes);
	cursor += bytes;

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
	memcpy(&uuid, cursor, bytes);
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

void GameObject::deleteComponent(Component* component)
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
			case ComponentType::SCRIPT:
				App->scripting->DestroyScript((ComponentScript*)component);
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

void GameObject::initAABB()
{
	ComponentMesh* Mesh = (ComponentMesh*)getComponentByType(ComponentType::MESH);

	if (Mesh && Mesh->mesh)
	{
		Mesh->updateGameObjectAABB();
	}
	else
	{
		if (parent)
		{
			Sphere sp;
			sp.pos = { 0,0,0 };
			sp.r = 0.2;
			initialAABB.SetNegativeInfinity();
			initialAABB.Enclose(sp);
		}
	}
}

AABB GameObject::getAABBChildsEnclosed()
{
	AABB bb;
	bb.SetNegativeInfinity();

	for (int i = 0; i < childs.size(); ++i)
	{
		bb.Enclose(childs[i]->getAABBChildsEnclosed());
	}

	bb.Enclose(boundingBox);

	return bb;
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
	BROFILER_CATEGORY("transformAABB", Profiler::Color::Azure);

	if (!initialAABB.IsFinite())
	{
		initAABB();
	}

	boundingBox.SetNegativeInfinity();

	boundingBox.Enclose(initialAABB);

	ComponentMesh* Mesh = (ComponentMesh*)getComponentByType(ComponentType::MESH);

	if (Mesh)
	{
		OBB obb(boundingBox);
		obb.Transform(this->transform->getGlobalMatrix().Transposed());
		boundingBox = obb.MinimalEnclosingAABB();
		updateAABBbuffers();
	}
	else
	{
		if (parent)
		{
			OBB obb(boundingBox);
			ComponentTransform global = transform->getGlobal();
			obb.Transform(ComponentTransform::composeMatrix(global.position, Quat(0, 0, 0, 1), float3(1, 1, 1)).Transposed());
			boundingBox = obb.MinimalEnclosingAABB();
			updateAABBbuffers();
		}
	}

	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->transformAABB();
	}

}