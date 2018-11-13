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
#include "ComponentMaterial.h"

#include <bitset>

#include "imgui/imgui_internal.h"
#include "imgui/imgui_stl.h"



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

	//App->fbxexporter->LoadFBX("Assets/meshes/BakerHouse.fbx");

	//Temp
	GameObject* camera = new GameObject(gameObjects.size() > 0 ? gameObjects[0] : nullptr);
	camera->CreateComponent(ComponentType::TRANSFORM);
	ComponentCamera* camcomp = (ComponentCamera*)camera->CreateComponent(ComponentType::CAMERA);
	camcomp->setMainCamera();
	camera->name = "MainCamera";
	gameObjects[0]->AddChild(camera);


	//----------------------INITIAL GRID------------------------
	grid.Init();
	grid.setColor(255, 255, 255, 0);
	euler.Init();

	return ret;
}

bool ModuleScene::Init()
{
	if (gameObjects.size() == 0)
	{
		GameObject* root = CreateGameObject(nullptr);
		root->name = "RootNode";
	}
	return true;
}

// Load assets
bool ModuleScene::CleanUp()
{
	Debug.Log("Unloading Intro scene");

	return true;
}

update_status ModuleScene::PreUpdate()
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::Azure)

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		gameObjects[0]->deleteSelected();
	}
	
	if (!quadtree.root.isInitialized())
		InitQuadtree();

	static float timer = 1 / quadTreeUpdateRate;
	if (timer <= 0)
	{
		UpdateQuadtree();
		timer = 1 / quadTreeUpdateRate;
	}
	timer -= App->time->dt;

	return update_status::UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update()
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::Azure)

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->Update(App->time->dt);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	BROFILER_CATEGORY("ModuleSceneIntro", Profiler::Color::DarkViolet)

	if (!IN_GAME)
	{
		DrawGuizmos();
	}

	//-------------------INITIAL GRID---------------------
	euler.Render();
	grid.Render();

	return update_status::UPDATE_CONTINUE;
}

void ModuleScene::InitQuadtree()
{
	quadtree.Create(gameObjects[0]->boundingBox);
	std::vector<GameObject*> decomposedGameObjects;
	decomposeScene(decomposedGameObjects);
	for (int i = 0; i < decomposedGameObjects.size(); ++i)
	{
		quadtree.Insert(decomposedGameObjects[i]);
	}
}

void ModuleScene::UpdateQuadtree()
{
	quadtree.Clear();
	quadtree.Resize(gameObjects[0]->getAABBChildsEnclosed());

	std::vector<GameObject*> gameObjects_s;
	decomposeScene(gameObjects_s);

	for (int i = 0; i < gameObjects_s.size(); ++i)
	{
		if (gameObjects_s[i] == App->camera->gameCamera)
			continue;

		quadtree.Insert(gameObjects_s[i]);
	}
}

void ModuleScene::DrawGuizmos()
{
	GameObject* selected = getSelectedGO();
	if (selected)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			currentGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
			currentGuizmoOperation = ImGuizmo::OPERATION::BOUNDS;
		else if(App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			currentGuizmoOperation = ImGuizmo::OPERATION::ROTATE;
		else if(App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			currentGuizmoOperation = ImGuizmo::OPERATION::SCALE;

		ImGuizmo::Enable(true);

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		math::float4x4 viewMatrix = App->camera->GetViewMatrix();
		math::float4x4 projectionMatrix = App->camera->GetProjMatrix();
		math::float4x4 transformMatrix = selected->transform->getGlobalMatrix();

		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), currentGuizmoOperation, guizmoMode, transformMatrix.ptr());

		if (ImGuizmo::IsUsing())
		{
			transformMatrix = transformMatrix.Transposed();
			selected->transform->setFromGlobalMatrix(transformMatrix);
			selected->transformAABB();
		}
	}
	else
	{
		ImGuizmo::Enable(false);
		currentGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
	GameObject* ret = new GameObject(parent);

	if(parent)
		parent->AddChild(ret);
	else
		gameObjects.push_back(ret);

	ret->CreateComponent(ComponentType::TRANSFORM); //All GameObjects have a Transform

	ret->initAABB();
	ret->transformAABB();
	ret->updateAABBbuffers();

	return ret;
}

void ModuleScene::AddGameObject(GameObject* gameObject)
{
	gameObjects[0]->AddChild(gameObject);
	gameObject->parent = gameObjects[0];
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

	//Hierarchy menu

	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("Hierarchy menu");
	}
	if (ImGui::BeginPopup("Hierarchy menu"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("GameObject"))
			{
				GameObject* gameObject = CreateGameObject(gameObjects[0]);
				gameObject->name = "GameObject";
			}
			if (ImGui::MenuItem("Camera"))
			{
				GameObject* gameObject = CreateGameObject(gameObjects[0]);
				gameObject->name = "Camera";
				gameObject->CreateComponent(ComponentType::CAMERA);
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
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
				ImGui::SetNextTreeNodeOpen(true);

			_ReorderGameObject_Pre(go);

			bool opened = ImGui::TreeNodeEx((go->name + "##" + std::to_string(go->uuid)).data(), flags);
			
			if (ImGui::IsItemClicked(0) && !go->selected)
				App->scene->selectGO(go);

			if (!opened)
				go->treeOpened = false;
			else
				go->treeOpened = true;

			DragDrop(go);

			_ReorderGameObject_Post(go);

			if(opened)
			{							
				for (int i = 0; i < go->childs.size(); ++i)
				{
					PrintHierarchy(go->childs[i]);					
				}			
				ImGui::TreePop();
			}
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;

			_ReorderGameObject_Pre(go);

			bool opened = ImGui::TreeNodeEx((go->name + "##" + std::to_string(go->uuid)).data(), flags);
			
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

void ModuleScene::debugDraw()
{
	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->recursiveDebugDraw();
	}

	//quadtree.Draw();
}

void ModuleScene::Serialize() const
{	
	char* buffer;
	uint size;

	SerializeToBuffer(buffer, size);
	App->fs->OpenWriteBuffer(SCENES_ASSETS_FOLDER + currentSceneName + SCENES_EXTENSION, buffer, size);

	delete buffer;
}

void ModuleScene::DeSerialize(std::string path, std::string extension)
{
	char* buffer;
	int size = 0u;
	if (!App->fs->OpenRead(path + extension, &buffer, size))
		return;

	int pos = path.find_last_of("/") != std::string::npos ? path.find_last_of("/") : path.find_last_of("\\");
	currentSceneName = path.substr(pos + 1);

	DeSerializeFromBuffer(buffer);

	delete buffer;
}

void ModuleScene::SerializeToBuffer(char*& buffer, uint& size) const
{
	std::vector<GameObject*> gameObject_s;
	std::vector<ComponentTransform*> transforms;
	std::vector<ComponentMesh*> meshes;
	std::vector <ComponentCamera*> cameras;
	std::vector<ComponentMaterial*> materials;

	decomposeScene(gameObject_s, transforms, meshes, cameras, materials);

	uint gameObjectsSize = 0u;
	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		gameObjectsSize += gameObject_s[i]->bytesToSerialize();
	}

	size = sizeof(uint) + gameObjectsSize + //Each gameobject's name has a different name lenght
		sizeof(uint) + ComponentTransform::bytesToSerialize() * transforms.size() +
		sizeof(uint) + ComponentMesh::bytesToSerialize() * meshes.size() +
		sizeof(uint) + ComponentCamera::bytesToSerialize() * cameras.size() +
		sizeof(uint) + ComponentMaterial::bytesToSerialize() * materials.size();

	buffer = new char[size];
	char* cursor = buffer;

	uint numGOs = gameObject_s.size();

	uint bytes = sizeof(uint);
	memcpy(cursor, &numGOs, bytes);
	cursor += bytes;

	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		gameObject_s[i]->Serialize(cursor);
	}

	uint numTransforms = transforms.size();

	bytes = sizeof(uint);
	memcpy(cursor, &numTransforms, bytes);
	cursor += bytes;

	for (int i = 0; i < transforms.size(); ++i)
	{
		transforms[i]->Serialize(cursor);
	}

	uint numMeshes = meshes.size();
	bytes = sizeof(uint);

	memcpy(cursor, &numMeshes, bytes);
	cursor += bytes;

	for (int i = 0; i < numMeshes; ++i)
	{
		meshes[i]->Serialize(cursor);
	}

	uint numCameras = cameras.size();
	bytes = sizeof(uint);
	memcpy(cursor, &numCameras, bytes);
	cursor += bytes;

	for (int i = 0; i < numCameras; ++i)
	{
		cameras[i]->Serialize(cursor);
	}

	uint numMaterials = materials.size();
	bytes = sizeof(uint);

	memcpy(cursor, &numMaterials, bytes);
	cursor += bytes;

	for (int i = 0; i < numMaterials; ++i)
	{
		materials[i]->Serialize(cursor);
	}

}

void ModuleScene::DeSerializeFromBuffer(char*& buffer)
{
	char* cursor = buffer;

	std::vector<GameObject*> gameObject_s;
	std::vector<ComponentTransform*> transforms;
	std::vector<ComponentMesh*> meshes;
	std::vector <ComponentCamera*> cameras;
	std::vector<ComponentMaterial*> materials;

	uint numGOs = 0;
	uint bytes = sizeof(uint);

	memcpy(&numGOs, cursor, bytes);
	cursor += bytes;

	std::vector<uint32_t> parentUUIDs;
	for (int i = 0; i < numGOs; ++i)
	{
		GameObject* newGO = new GameObject(nullptr);
		uint32_t parentUUID;
		newGO->DeSerialize(cursor, parentUUID);
		parentUUIDs.push_back(parentUUID);
		gameObject_s.push_back(newGO);
	}

	for (int i = 0; i < gameObject_s.size(); ++i) //For each gameobject
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (parentUUIDs[i] == gameObject_s[j]->uuid) //Look for a gameobject whose UUID is the same as your parent
			{
				gameObject_s[i]->parent = gameObject_s[j]; //Create this parent-child relationship
				gameObject_s[j]->AddChild(gameObject_s[i]);
			}
		}
	}

	uint numTransforms = 0u;
	bytes = sizeof(uint);
	memcpy(&numTransforms, cursor, bytes);
	cursor += bytes;

	std::vector<uint32_t> goUUIDs;

	for (int i = 0; i < numTransforms; ++i)
	{
		ComponentTransform* newTransform = new ComponentTransform();
		uint32_t goUUID;
		newTransform->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		transforms.push_back(newTransform);
	}

	for (int i = 0; i < transforms.size(); ++i) //For each transform
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid) //Look for a gameobject whose UUID is the same as your one
			{
				transforms[i]->gameObject = gameObject_s[j]; //Create this gameobject-component relationship
				gameObject_s[j]->AddComponent(transforms[i]);
				gameObject_s[j]->transform = transforms[i];
			}
		}
	}

	uint numMeshes = 0u;
	bytes = sizeof(uint);

	memcpy(&numMeshes, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numMeshes; ++i)
	{
		ComponentMesh* newMesh = App->renderer3D->CreateComponentMesh(nullptr);
		uint32_t goUUID;
		newMesh->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		meshes.push_back(newMesh);
	}

	for (int i = 0; i < meshes.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				meshes[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(meshes[i]);
			}
		}
	}

	uint numCameras = 0u;
	bytes = sizeof(uint);
	memcpy(&numCameras, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	bool mainCamera = false;

	for (int i = 0; i < numCameras; ++i)
	{
		ComponentCamera* newCamera = new ComponentCamera(nullptr);
		uint32_t goUUID;
		newCamera->DeSerialize(cursor, goUUID);
		if (!mainCamera && newCamera->isMainCamera)
		{
			mainCamera = true;
			App->camera->setGameCamera(newCamera);
		}		
		goUUIDs.push_back(goUUID);
		cameras.push_back(newCamera);
	}
	if (!mainCamera)
	{
		App->camera->setGameCamera(nullptr);
	}
	for (int i = 0; i < cameras.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				cameras[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(cameras[i]);
				cameras[i]->updateFrustum();
			}
		}
	}

	uint numMaterials = 0u;
	bytes = sizeof(uint);

	memcpy(&numMaterials, cursor, bytes);
	cursor += bytes;
	goUUIDs.clear();

	for (int i = 0; i < numMaterials; ++i)
	{
		ComponentMaterial* newMaterial = new ComponentMaterial(nullptr);
		uint32_t goUUID;
		newMaterial->DeSerialize(cursor, goUUID);
		goUUIDs.push_back(goUUID);
		materials.push_back(newMaterial);
	}

	for (int i = 0; i < materials.size(); ++i)
	{
		for (int j = 0; j < gameObject_s.size(); ++j)
		{
			if (goUUIDs[i] == gameObject_s[j]->uuid)
			{
				materials[i]->gameObject = gameObject_s[j];
				gameObject_s[j]->AddComponent(materials[i]);
			}
		}
	}

	delete gameObjects[0];
	gameObjects.clear();

	GameObject* root = nullptr;
	for (int i = 0; i < gameObject_s.size(); ++i)
	{
		if (gameObject_s[i]->parent == nullptr)
		{
			root = gameObject_s[i];
			break;
		}
	}

	gameObjects.push_back(root);
}

void ModuleScene::TransformGUI()
{
	ImVec2 drawingPos = ImGui::GetCursorScreenPos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGui::SetCursorScreenPos({ windowPos.x + 7.0f,windowPos.y + 2.0f });

	if (guizmoMode == ImGuizmo::MODE::WORLD)
	{
		if (ImGui::Button("Global", { 50,28 }))
		{
			guizmoMode = ImGuizmo::MODE::LOCAL;
		}
	}
	else
	{
		if (ImGui::Button("Local", { 50,28 }))
		{
			guizmoMode = ImGuizmo::MODE::WORLD;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Tra(W)", { 50,28 }))
	{
		currentGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rot(E)", { 50,28 }))
	{
		currentGuizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Sca(R)", { 50,28 }))
	{
		currentGuizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
}

AABB ModuleScene::getSceneAABB() const
{
	return gameObjects.size() > 0 ? gameObjects[0]->boundingBox : AABB(-float3::inf, -float3::inf); 
}

void ModuleScene::OnRenameGUI()
{
	if (ImGui::BeginPopup("#### Renaming Scene"))
	{
		static std::string temp = currentSceneName;

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos({cursorPos.x, cursorPos.y + 6});
		ImGui::Text("New Name: "); ImGui::SameLine();
		cursorPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos({ cursorPos.x, cursorPos.y - 6});
		ImGui::InputText("##RenamingSceneName", &temp);

		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			currentSceneName = temp;
			ImGui::CloseCurrentPopup();
		}
			
		ImGui::EndPopup();
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

void ModuleScene::decomposeScene(std::vector<GameObject*>&gameObject_s, std::vector<ComponentTransform*>&transforms, std::vector<ComponentMesh*>&meshes, 
								 std::vector<ComponentCamera*>&cameras, std::vector<ComponentMaterial*>&materials) const
{
	gameObjects[0]->Decompose(gameObject_s, transforms, meshes, cameras, materials);
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