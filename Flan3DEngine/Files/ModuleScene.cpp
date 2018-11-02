#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "Brofiler\Brofiler.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "Component.h"

#include <bitset>

#include "imgui/imgui_internal.h"



#define CHECKERS 8 * 8

ModuleScene::ModuleScene(bool start_enabled) : Module("ModuleSceneIntro", start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	Debug.Log("Loading Intro assets");
	bool ret = true;

	/*App->camera->Move(float3(0.0f, 5.0f, 5.0f));
	App->camera->LookAt(float3(0, 0, 0));*/


	//-------------SPAWN HOUSE WITH THE TEXTURE-----------
	App->fbxLoader->LoadFBX("Assets/meshes/BakerHouse.fbx");
	App->textures->LoadTexture("Assets/textures/Baker_house.dds");

	//Temp
	GameObject* camera = new GameObject(gameObjects[0]);
	camera->CreateComponent(ComponentType::TRANSFORM);
	camera->CreateComponent(ComponentType::CAMERA);
	camera->name = "MainCamera";
	gameObjects[0]->InsertChild(camera, 0);

	//----------------------INITIAL GRID------------------------
	grid.Init();
	grid.setColor(255, 255, 255, 0);
	euler.Init();

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	Debug.Log("Unloading Intro scene");

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		gameObjects[0]->deleteSelected();
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		Serialize("Assets/Scenes/exampleScene", ".flanScene");
	}
	return update_status::UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update(float dt)
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->Update(dt);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleSceneIntro", Profiler::Color::DarkViolet)

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();

	if (App->debugDraw)
	{
		debugDraw();
	}
	return update_status::UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* ret = new GameObject(parent);

	if(parent)
		parent->AddChild(ret);
	else
		gameObjects.push_back(ret);

	ret->CreateComponent(ComponentType::TRANSFORM); //All GameObjects have a Transform

	return ret;
}

void ModuleScene::guiHierarchy()
{
	PrintHierarchy(gameObjects[0]);

	ImVec2 pos = ImGui::GetCursorPos();
	ImGui::Dummy({ ImGui::GetWindowSize().x - pos.x,  ImGui::GetWindowSize().y - pos.y});
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		GameObject* selected = getSelectedGO();
		if (selected)
			selected->selected = false;
	}

	//Drop on window to unparent a GameObject
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags flags = 0;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingGOs", flags);
		if (payload)
		{
			GameObject* other = *(GameObject**)payload->Data;

			other->transform->setLocal(&other->transform->getGlobal());
			other->parent->ClearChild(other);
			other->parent = gameObjects[0];
			other->parent->AddChild(other);
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::PrintHierarchy(GameObject* go)
{
	bool print = true;

	if (go == gameObjects[0]) //Dont print the rootNode
		print = false;
	
	if (print)
	{
		int flags = 0;

		if (go->selected)
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

		if (go->HasChilds())
		{								
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;

			if (go->treeOpened)
				flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen;

			_ReorderGameObject_Pre(go);

			bool opened = ImGui::TreeNodeEx(go->name.data(), flags);
			
			if (ImGui::IsItemClicked(0) && !go->selected)
				App->scene->selectGO(go);

			DragDrop(go);

			_ReorderGameObject_Post(go);

			if(opened)
			{			
				go->treeOpened = true;
				for (int i = 0; i < go->childs.size(); ++i)
				{
					PrintHierarchy(go->childs[i]);					
				}			
				ImGui::TreePop();
			}
			else
			{
				go->treeOpened = false;
			}
			
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;

			_ReorderGameObject_Pre(go);

			bool opened = ImGui::TreeNodeEx(go->name.data(), flags);
			
			if (ImGui::IsItemClicked(0) && ImGui::IsItemHovered(0) && !go->selected)
				App->scene->selectGO(go);
			
			DragDrop(go);

			_ReorderGameObject_Post(go);

			if(opened)
			{
				ImGui::TreePop();
			}
		}
	}
	else
	{
		//RootNode only
		if (go->HasChilds())
		{
			for (int i = 0; i < go->childs.size(); ++i)
			{
				PrintHierarchy(go->childs[i]);		
			}					
		}
	}
}

void ModuleScene::ClearGameObjects()
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		delete gameObjects[i];
	}
	gameObjects.clear();
}

void ModuleScene::guiInspector()
{
	GameObject* selected = getSelectedGO();
	if (!selected)
	{
		ImGui::Text("No GameObjects selected");
		return;
	}	
	selected->OnInspector();
}

GameObject* ModuleScene::getSelectedGO() const
{
	GameObject* ret = nullptr;
	
	for (int i = 0; i < gameObjects.size() && !ret; ++i)
	{
		ret = gameObjects[i]->getSelectedGO();
	}

	return ret;
}

void ModuleScene::selectGO(GameObject* toSelect)
{
	GameObject* selected = getSelectedGO();
	if (selected)
	{
		selected->selected = false;
	}
	toSelect->selected = true;
}

void ModuleScene::debugDraw() const
{
	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->recursiveDebugDraw();
	}
}

void ModuleScene::Serialize(std::string path, std::string extension)
{
	std::vector<GameObject*> gameObject_s;
	std::vector<ComponentTransform*> transforms;
	std::vector<ComponentMesh*> meshes;
	std::vector <ComponentCamera*> cameras;

	decomposeScene(gameObject_s, transforms, meshes, cameras);

	//num components each type (Only cameras / meshes), each component UUID, transform size
	uint gameObject_size = ComponentTransform::bytesToSerialize() + sizeof(uint[2]) + sizeof(uint32_t[2]); //TODO: COMPLETE THAT TO ANY NUMBER OF COMPONENTS

	//Num gameobjects + num gameobjects * each gameobject size
	uint size = sizeof(uint) + gameObject_s.size() * gameObject_size;

	char* buffer = new char[size];
	char* cursor = buffer;
	uint bytes = 0u;
	   	  
	for(int i = 0; i < gameObject_s.size(); ++i)
	{
		//For each gameobject
		gameObject_s[i]->transform->Serialize(cursor);

		uint numComponentByType[2];
		uint32_t UUIDs[2];

		//For each component
		for (uint component = 0; component < gameObject_s[i]->components.size(); ++component) 
		{
			switch (gameObject_s[i]->components[component]->type)
			{
			case ComponentType::MESH:
				numComponentByType[0]++;
				UUIDs[0] = gameObject_s[i]->components[component]->UUID;
				break;
			case ComponentType::CAMERA:
				numComponentByType[1]++;
				UUIDs[1] = gameObject_s[i]->components[component]->UUID;
				break;
			}
		}

		memcpy(cursor, numComponentByType, sizeof(numComponentByType));
		cursor += sizeof(numComponentByType);
		memcpy(cursor, UUIDs, sizeof(UUIDs));
		cursor += sizeof(UUIDs);
	}

	//TODO: SAVE THE FILE

	//Serialize the meshes
	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->Serialize();
	}

	//TODO: Serialize the cameras
	for (int i = 0; i < cameras.size(); ++i)
	{
		//cameras[i]->Serialize();
	}
}

void ModuleScene::DragDrop(GameObject* go)
{
	//Drag and drop

	bool abortDrop = false;

	ImGuiDragDropFlags src_flags = 0;
	//src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
	src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;

	if (ImGui::BeginDragDropSource(src_flags))
	{
		ImGui::BeginTooltip();
		ImGui::Text(go->name.data());
		ImGui::EndTooltip();
		ImGui::SetDragDropPayload("DraggingGOs", &go, sizeof(GameObject));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingGOs");
		if (payload)
		{
			GameObject* other = *(GameObject**)payload->Data;

			if (other->parent == go)
				abortDrop = true;

			

			if (!abortDrop)
			{
				//Delete this object from the parent childs
				other->parent->ClearChild(other);
						
				if (go->parent == other)
				{
					//When swapping familiars we have to alert the grandpa
					other->parent->AddChild(go);

					//Update the parent of the newParent:
					go->transform->setLocalWithParent(other->parent->transform);
					go->parent = other->parent;

					other->ClearChild(go);
				}
				
				//Change the transform to correspond with the new parent
				other->transform->setLocalWithParent(go->transform); 
			
				//Set the new parent
				other->parent = go;

				//Add the new child to the childs array of the new parent
				go->AddChild(other);

				//Start the parent TreeNode opened
				go->treeOpened = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::decomposeScene(std::vector<GameObject*>&gameObject_s, std::vector<ComponentTransform*>&transforms, std::vector<ComponentMesh*>&meshes, std::vector<ComponentCamera*>&cameras)
{
	gameObjects[0]->Decompose(gameObject_s, transforms, meshes, cameras);
}

void ModuleScene::_ReorderGameObject_Pre(GameObject* go)
{
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImGui::Dummy({ ImGui::GetWindowWidth(), 5 });
	ImGui::SetCursorScreenPos(cursorPos);
	if (ImGui::BeginDragDropTarget())
	{		
		ImGuiDragDropFlags flags = 0;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingGOs", flags);
		
		if (payload)
		{
			GameObject* other = *(GameObject**)payload->Data;

			//For now we only support the reordering between childs. TODO:
			if (other->parent == go->parent && other != go)
			{
				bool draw = false;
				int pos = go->parent->getChildPos(go);
				if ((pos != 0 && go->parent->childs[pos - 1] != other) || pos == 0) //Check if the one you want to place above you is already there
					draw = true;
				
				if (draw)
				{
					//Draw a line
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					ImGui::ItemSize(ImVec2(0.0f, 0.0f));
					float x1 = window->Pos.x;
					float x2 = window->Pos.x + window->Size.x;
					ImRect rec{ ImVec2(x1, window->DC.CursorPos.y - 5), ImVec2(x2, window->DC.CursorPos.y - 5) };
					window->DrawList->AddLine(rec.Min, ImVec2(rec.Max.x, rec.Min.y), ImGui::GetColorU32(ImGuiCol_::ImGuiCol_DragDropTarget), 2);

					if (ImGui::IsMouseReleased(0))
					{
						other->parent->ClearChild(other);
						other->parent->InsertChild(other, other->parent->getChildPos(go));
					}
				}			
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleScene::_ReorderGameObject_Post(GameObject* go)
{
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImGui::Dummy({ ImGui::GetWindowWidth(), 5 });
	ImGui::SetCursorScreenPos(cursorPos);
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags flags = 0;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
		flags |= ImGuiDragDropFlags_::ImGuiDragDropFlags_AcceptBeforeDelivery;
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DraggingGOs", flags);

		if (payload)
		{
			GameObject* other = *(GameObject**)payload->Data;
			//For now we only support the reordering between childs. TODO:
			if (other->parent == go->parent && other != go)
			{		
				int pos = go->parent->getChildPos(go);
				if (pos == go->parent->childs.size() - 1)
				{
					//Draw a line
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					ImGui::ItemSize(ImVec2(0.0f, 0.0f));
					float x1 = window->Pos.x;
					float x2 = window->Pos.x + window->Size.x;
					ImRect rec{ ImVec2(x1, window->DC.CursorPos.y - 5), ImVec2(x2, window->DC.CursorPos.y - 5) };
					window->DrawList->AddLine(rec.Min, ImVec2(rec.Max.x, rec.Min.y), ImGui::GetColorU32(ImGuiCol_::ImGuiCol_DragDropTarget), 2);
				
					if (ImGui::IsMouseReleased(0))
					{
						other->parent->ClearChild(other);
						other->parent->InsertChild(other, other->parent->getChildPos(go) + 1);
					}
				}				
			}
		}
		ImGui::EndDragDropTarget();
	}
}