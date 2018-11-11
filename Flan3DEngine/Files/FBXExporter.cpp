#include "FBXExporter.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "assimp/include/scene.h"
#include "assimp/include/postprocess.h"
#include "assimp/include/cfileio.h"

#include "Glew/include/glew.h"

#include "MathGeoLib_1.5/MathGeoLib.h"

#include <string>

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"


#pragma comment (lib, "Assimp/libx86/assimp-vc140-mt.lib")

#include "ComponentMesh.h"
#include "GameObject.h"

void LogCallback(const char* message, char* user)
{
	Debug.enter = false;
	std::string log(message);
	if(log.find("Error") != std::string::npos)
		Debug.LogError("Assimp Error: %s", message);
	else
		Debug.Log("Assimp Log: %s", message);
	Debug.enter = true;
}

bool FBXExporter::Start()
{
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool FBXExporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

update_status FBXExporter::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXExporter::Update()
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXExporter::PostUpdate()
{
	
	return update_status::UPDATE_CONTINUE;
}

//Save changes the JSON, not the module
bool FBXExporter::Save(JSON_Object* obj) const
{
	return true;
}

//Load changes the module, not the JSON
bool FBXExporter::Load(const JSON_Object* obj)
{
	return true;
}

bool FBXExporter::LoadFBX(char* path, bool useFS)
{
	//bool ret = true;
	//int size_buffer = 0;
	//char* buffer;
	//const aiScene* scene;
	//if (useFS)
	//{
	//	if (!App->fs->OpenRead(path, &buffer, size_buffer))
	//	{
	//		Debug.LogError("Loading geometries failed, FileSystem reported an error");
	//		return false;
	//	}
	//	else
	//	{
	//		scene = aiImportFileFromMemory(buffer, size_buffer, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	//	}
	//}	
	//else
	//{
	//	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	//}
	// 
	//if (scene != nullptr && scene->HasMeshes())
	//{
	//	App->scene->ClearGameObjects();
	//	App->renderer3D->ReserveMeshes(scene->mNumMeshes); //Only allocate memory once

	//	GameObject* root = App->scene->CreateGameObject(nullptr);
	//	aiVector3D pos, scale;
	//	aiQuaternion rotation;
	//	scene->mRootNode->mTransformation.Decompose(scale, rotation, pos);
	//	root->transform->position = { pos.x, pos.y, pos.z };
	//	root->transform->rotation = { rotation.x,rotation.y,rotation.z,rotation.w };
	//	root->transform->scale = { scale.x,scale.y,scale.z };
	//	root->name = scene->mRootNode->mName.C_Str();
	//	
	//	for (int i = 0; i < scene->mRootNode->mNumMeshes; ++i) //for each mesh in the root node
	//	{
	//		ComponentMesh* meshComp = (ComponentMesh*)root->CreateComponent(ComponentType::MESH); //create a new ComponentMesh and add it to the root node
	//		FillMeshData(meshComp, scene->mMeshes[scene->mRootNode->mMeshes[i]]); //Initialize that MeshComponent with the corresponding aiMesh
	//		meshComp->genBuffers();
	//		Debug.Log("New mesh loaded with %d vertex", meshComp->num_vertex);
	//	}
	//	
	//	RecursivelyHierarchy(scene->mRootNode, root, scene);

	//	aiReleaseImport(scene);
	//	App->renderer3D->CalculateSceneBoundingBox();
	//}
	//else
	//{
	//	Debug.LogError("Error loading the scene %s. Error: %s", path, aiGetErrorString());
	//	ret = false;
	//}
	return true;
}

bool FBXExporter::RecursivelyHierarchy(const aiNode* parent, const GameObject* parentGO, const aiScene* scene)
{
	//int i = 0;

	//if(parent->mNumChildren > 0)
	//for (aiNode* node = parent->mChildren[0]; i < parent->mNumChildren; ++i, node = parent->mChildren[i])
	//{
	//	GameObject* newGO = App->scene->CreateGameObject((GameObject*)parentGO); //Generate a new GO for each child
	//	
	//	aiVector3D pos, scale;
	//	aiQuaternion rotation;
	//	node->mTransformation.Decompose(scale, rotation, pos);
	//	newGO->transform->position = { pos.x, pos.y, pos.z };
	//	newGO->transform->rotation = { rotation.x,rotation.y,rotation.z,rotation.w };
	//	newGO->transform->scale = { scale.x,scale.y,scale.z };
	//	newGO->name = node->mName.C_Str();

	//	for (int j = 0; j < node->mNumMeshes; ++j) //Generate a new MeshComponent for each mesh in child
	//	{
	//		ComponentMesh* meshComp = (ComponentMesh*)newGO->CreateComponent(ComponentType::MESH);
	//		FillMeshData(meshComp, scene->mMeshes[node->mMeshes[j]]);
	//		meshComp->genBuffers();
	//		newGO->boundingBox.Enclose((float3*)meshComp->vertex, meshComp->num_vertex);
	//		newGO->updateAABBbuffers();
	//		Debug.Log("New mesh loaded with %d vertex", meshComp->num_vertex);
	//	}
	//	RecursivelyHierarchy(node, newGO, scene); //Repeat it for each child
	//}
	return true;
}

bool FBXExporter::FillMeshData(ComponentMesh* mymesh, aiMesh* mesh)
{
	//mymesh->num_vertex = mesh->mNumVertices;
	//mymesh->vertex = new float[mesh->mNumVertices * 3];
	//memcpy(mymesh->vertex, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3); //Init my vertex with the vertices array

	//if (mesh->HasFaces())
	//{
	//	mymesh->num_index = mesh->mNumFaces * 3; //Assuming each face has 3 vertices => They all are triangles.
	//	mymesh->index = new uint[mymesh->num_index];
	//	for (uint j = 0; j < mesh->mNumFaces; ++j)
	//	{
	//		if (mesh->mFaces[j].mNumIndices != 3)
	//			Debug.LogWarning("Charging a geometry face with != 3 vertices! Some errors might happen");
	//		else // This face has 3 vertex

	//			memcpy(&mymesh->index[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint)); //copy the 3 index into the right place in our index array							
	//	}
	//}

	//if (mesh->HasNormals())
	//{
	//	mymesh->normals = new float[mymesh->num_vertex * 3];
	//	memcpy(mymesh->normals, mesh->mNormals, sizeof(float) * mymesh->num_vertex * 3);
	//}

	//int colorChannels = mesh->GetNumColorChannels();
	//if (colorChannels > 0)
	//{
	//	for (int i = 0; i < colorChannels; ++i)
	//	{
	//		if (mesh->HasVertexColors(i))
	//		{
	//			mymesh->colors = new float[mymesh->num_vertex * 4];
	//			memcpy(mymesh->colors, mesh->mColors[i], sizeof(float) * mymesh->num_vertex * 4);
	//			break; //We only keep support of 1 set of colors, for now//
	//		}
	//	}
	//}

	//if (mesh->HasTextureCoords(0))
	//{
	//	mymesh->textureCoords = new float[mymesh->num_vertex * 2];
	//	for (int j = 0; j < mymesh->num_vertex; ++j)
	//	{
	//		memcpy(&mymesh->textureCoords[j * 2], &mesh->mTextureCoords[0][j].x, sizeof(float));
	//		memcpy(&mymesh->textureCoords[(j * 2) + 1], &mesh->mTextureCoords[0][j].y, sizeof(float));
	//	}
	//}	

	//mymesh->name = mesh->mName.C_Str();

	//Save an own-formatted mesh file
	//SaveMesh(mymesh);

	return true;
}

std::vector<Resource*> FBXExporter::ExportFBX(const std::string& file) const
{
	std::vector<Resource*> ret;

	char* buffer;
	int size;
	if (!App->fs->OpenRead(file, &buffer, size))
	{
		return ret;
	}

	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	if (!scene || !scene->HasMeshes())
	{
		Debug.LogError("Error loading the scene %s. Error: %s", file, aiGetErrorString());
		delete buffer;
		return ret;
	}
		
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Now we have a bunch of data in the scene. We want to export all that the fbx contains in different files in Library, keeping the conection in a .meta file.
	//We also need to keep somewhere the node-structure, in order to be able of instanciating this fbx. 
	//Options: Inside the .meta? Hidden prefab in Library, linked with the .meta, as our Scene's style? We will store that in the .meta because of for now we have
	//		   no data to store there. 

	//Extras: We need to give the user the capability to access the different meshes and textures, and reference them inside components, as if they were
	//separated files.

	//We do not support Embedded Textures for now
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Storage the node-structure
	std::vector<const aiNode*> decomposedHierarchy = decomposeAssimpHierarchy(scene->mRootNode);
	decomposedHierarchy.insert(decomposedHierarchy.begin(), (const aiNode*)scene->mRootNode);

	//We can store in Library a file containing fake-UIDs for each aiNode referencing to their parent, 
	//and keep their resources UID.

	//Fake UIDS
	UID* uids = new UID[decomposedHierarchy.size()];
	for (int i = 0; i < decomposedHierarchy.size(); ++i)
	{
		uids[i] = FLAN::randomUINT32_Range();
	}

	//Parent for each aiNode
	UID* parent_uids = new UID[decomposedHierarchy.size()];
	for (int i = 0; i < decomposedHierarchy.size(); ++i)
	{
		for (int j = 0; j < decomposedHierarchy.size(); ++j)
		{
			if (decomposedHierarchy[i]->mParent == decomposedHierarchy[j])
			{
				parent_uids[i] = uids[j];
			}
		}
	}

	std::vector<ResourceTexture*> textures;
	std::map<const aiMesh*, ResourceMesh*> meshes;
	
	uint fileSize = sizeof(uint) + //Num aiNodes
		decomposedHierarchy.size() *
		(
			sizeof(char[100]) + //aiNode Name
			sizeof(aiVector3D) * 2 + //Position, scale
			sizeof(aiQuaternion) + //Rotation
			sizeof(UID) * 4 //Your uid, your parent's uid, your texture uid and your mesh uid
		);
	
	char* hierarchyBuffer = new char[fileSize];
	char* cursor = hierarchyBuffer;

	uint numaiNodes = decomposedHierarchy.size();
	memcpy(cursor, &numaiNodes, sizeof(uint));
	cursor += sizeof(uint);

	for (int i = 0; i < decomposedHierarchy.size(); ++i)
	{
		//Scan all the resources this aiNode is referencing

		UID meshUID = 0;
		UID textureUID = 0;

		if (decomposedHierarchy[i]->mNumMeshes > 0)
		{
			//We only support 1 mesh each gameObject
			const aiMesh* assimpMesh = scene->mMeshes[decomposedHierarchy[i]->mMeshes[0]];
			
			aiMaterial* assimpMaterial = scene->mMaterials[assimpMesh->mMaterialIndex];
			std::string texturePath;
			if (assimpMaterial)
			{
				aiString temp;
				assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &temp); //TODO: SPECULAR, EMMISIVE, NORMAL MAPS, ETC?
				texturePath = temp.C_Str();

				if (!texturePath.empty())
				{
					//Normalize the path
					while (texturePath.find_first_of("\\") != std::string::npos) /*First of \ */
					{
						uint pos = texturePath.find_first_of("\\");
						texturePath.replace(pos, 1, "/");
					}

					//Manage ".."

					uint lastPos = file.find_last_of("/");
					std::string path = file.substr(0, lastPos + 1);

					bool prevDirFound = texturePath.find("..") != std::string::npos;

					if (prevDirFound)
					{
						//Delete "lastDir/" from the fbx path and "../" from the texturePath
						uint lastSlashPos = path.find_last_of("/");
						path.erase(lastSlashPos);
						lastSlashPos = path.find_last_of("/");
						path.replace(lastSlashPos + 1, std::string::npos, "");

						uint texturePos = texturePath.find("..");
						texturePath.replace(texturePos, 3, "");
					}

					texturePath.insert(0, path);
				}
				
			}

			//Now we have a Mesh and a Texture. Save them in our Resources vector

			//Store the mesh, if this has not been saved yet


			ResourceMesh* savedMesh = meshes.find(assimpMesh) != meshes.end() ? meshes.at(assimpMesh) : nullptr;

			if(!savedMesh)
			{
				ResourceMesh* myMesh = new ResourceMesh();
				meshUID = myMesh->getUUID();
				//Store vertex
				myMesh->num_vertex = assimpMesh->mNumVertices;
				myMesh->vertex = new float[myMesh->num_vertex * 3];
				memcpy(myMesh->vertex, assimpMesh->mVertices, sizeof(float) * myMesh->num_vertex * 3);

				//Store index
				if (assimpMesh->HasFaces())
				{
					myMesh->num_index = assimpMesh->mNumFaces * 3; //Assuming each face has 3 vertices => They all are triangles.
					myMesh->index = new uint[myMesh->num_index];
					for (uint j = 0; j < assimpMesh->mNumFaces; ++j)
					{
						if (assimpMesh->mFaces[j].mNumIndices != 3)
							Debug.LogWarning("Charging a geometry face with != 3 vertices! Some errors might happen");
						else // This face has 3 vertex
							memcpy(&myMesh->index[j * 3], assimpMesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				//Store Normals
				if (assimpMesh->HasNormals())
				{
					myMesh->normals = new float[myMesh->num_vertex * 3];
					memcpy(myMesh->normals, assimpMesh->mNormals, sizeof(float) * myMesh->num_vertex * 3);
				}

				//Store the first set of colors
				if (assimpMesh->HasVertexColors(0))
				{
					myMesh->colors = new float[myMesh->num_vertex * 4];
					memcpy(myMesh->colors, assimpMesh->mColors[i], sizeof(float) * myMesh->num_vertex * 4);
				}

				//Store the first set of UVs
				if (assimpMesh->HasTextureCoords(0))
				{
					myMesh->textureCoords = new float[myMesh->num_vertex * 2]; //Assimp stores a 3D vector for texture coordinates, we only support 2D ones
					for (int j = 0; j < myMesh->num_vertex; ++j)
					{
						memcpy(&myMesh->textureCoords[j * 2], &assimpMesh->mTextureCoords[0][j].x, sizeof(float));
						memcpy(&myMesh->textureCoords[(j * 2) + 1], &assimpMesh->mTextureCoords[0][j].y, sizeof(float));
					}
				}

				myMesh->genBuffers();
				myMesh->meshName = assimpMesh->mName.C_Str();
				ret.push_back(myMesh);
				meshes.insert(std::pair<const aiMesh*, ResourceMesh*>(assimpMesh, myMesh));

				char* meshBuffer = new char[myMesh->bytesToSerialize()];
				char* cursor = meshBuffer;
				myMesh->Serialize(cursor);
				App->fs->OpenWriteBuffer(MESHES_LIBRARY_FOLDER + std::string("/") + std::to_string(myMesh->getUUID()) + MESHES_EXTENSION, meshBuffer, myMesh->bytesToSerialize());
				delete meshBuffer;
			}
			else
			{
				meshUID = savedMesh->getUUID();
			}

			//Store the texture

			if (!texturePath.empty())
			{
				bool alreadyStored = false;

				//First, search in the previously stored Resources
				Resource* myTexture = App->resources->FindByFile((char*)texturePath.data());

				if (myTexture)
				{
					alreadyStored = true;
					textureUID = myTexture->getUUID();
				}					

				if (!alreadyStored)
				{
					for (int j = 0; j < textures.size(); ++j)
					{
						if (textures[j]->getFile() == texturePath)
						{
							alreadyStored = true;
							textureUID = textures[j]->getUUID();
						}
					}
				}				

				if (!alreadyStored)
				{
					myTexture = App->textures->ExportResource(texturePath);
					if (myTexture)
					{
						textures.push_back((ResourceTexture*)myTexture);
						ret.push_back(myTexture);
						textureUID = myTexture->getUUID();
					}
				}
			}			
		}

		//Save here the uuids: This one's, parent's, textures and meshes + transform

		std::string name = decomposedHierarchy[i]->mName.C_Str();
		if (name.length() >= 100)
			Debug.LogWarning("A file in the FBX hierarchy has a name greater than 100 chars. Wow. Some info may be missing");

		name = name.substr(0, 100);
		name.resize(100);

		aiVector3D pos, scale;
		aiQuaternion rotation;
		decomposedHierarchy[i]->mTransformation.Decompose(scale, rotation, pos);
		
		uint uidBytes = sizeof(UID);

		//Your name
		memcpy(cursor, name.c_str(), 100);
		cursor += 100;

		//Your uid
		memcpy(cursor, &uids[i], uidBytes);
		cursor += uidBytes;

		//Your parent's uid
		memcpy(cursor, &parent_uids[i], uidBytes);
		cursor += uidBytes;

		//Transform
		uint bytes = sizeof(aiVector3D);
		memcpy(cursor, &pos, bytes);
		cursor += bytes;

		Quat mathRot; //Assimp saves the quaternions in different orders than MathGeoLib
		mathRot.x = rotation.x;
		mathRot.y = rotation.y;
		mathRot.z = rotation.z;
		mathRot.w = rotation.w;

		bytes = sizeof(Quat);
		memcpy(cursor, &mathRot, bytes);
		cursor += bytes;

		bytes = sizeof(aiVector3D);
		memcpy(cursor, &scale, bytes);
		cursor += bytes;

		//Mesh / Texture UID
		memcpy(cursor, &meshUID, uidBytes);
		cursor += uidBytes;

		memcpy(cursor, &textureUID, uidBytes);
		cursor += uidBytes;
	}

	//Save the stored hierarchy in a .meta file
	App->fs->OpenWriteBuffer(file + ".meta", hierarchyBuffer, fileSize);

	aiReleaseImport(scene);
	delete buffer;
	delete hierarchyBuffer;

	return ret;
}

std::vector<const aiNode*> FBXExporter::decomposeAssimpHierarchy(const aiNode* rootNode) const
{
	std::vector<const aiNode*> ret;

	//First store all your childs
	for (int i = 0; i < rootNode->mNumChildren; ++i)
	{
		ret.push_back(rootNode->mChildren[i]);
	}

	//Then ask each child to store all their childs
	for (int i = 0; i < rootNode->mNumChildren; ++i)
	{
		std::vector<const aiNode*> childRet = decomposeAssimpHierarchy(rootNode->mChildren[i]);
		for (int j = 0; j < childRet.size(); ++j)
		{
			ret.push_back(childRet[j]);
		}
	}

	return ret;
}
