#include "FBXLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "assimp/include/scene.h"
#include "assimp/include/postprocess.h"
#include "assimp/include/cfileio.h"

#include "Glew/include/glew.h"

#include "MathGeoLib_1.5/MathGeoLib.h"

#include <string>

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

bool FBXLoader::Start()
{
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool FBXLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

update_status FBXLoader::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXLoader::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status FBXLoader::PostUpdate(float dt)
{
	
	return update_status::UPDATE_CONTINUE;
}

//Save changes the JSON, not the module
bool FBXLoader::Save(JSON_Object* obj) const
{
	return true;
}

//Load changes the module, not the JSON
bool FBXLoader::Load(const JSON_Object* obj)
{
	return true;
}

bool FBXLoader::LoadFBX(char* path, bool useFS)
{
	bool ret = true;
	int size_buffer = 0;
	char* buffer;
	const aiScene* scene;
	if (useFS)
	{
		if (!App->fs->OpenRead(path, &buffer, size_buffer))
		{
			Debug.LogError("Loading geometries failed, FileSystem reported an error");
			return false;
		}
		else
		{
			scene = aiImportFileFromMemory(buffer, size_buffer, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
		}
	}	
	else
	{
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}
	 
	if (scene != nullptr && scene->HasMeshes())
	{
		App->scene->ClearGameObjects();
		App->renderer3D->ReserveMeshes(scene->mNumMeshes); //Only allocate memory once

		GameObject* root = App->scene->CreateGameObject(nullptr);
		aiVector3D pos, scale;
		aiQuaternion rotation;
		scene->mRootNode->mTransformation.Decompose(scale, rotation, pos);
		root->transform->position = { pos.x, pos.y, pos.z };
		root->transform->rotation = { rotation.x,rotation.y,rotation.z,rotation.w };
		root->transform->scale = { scale.x,scale.y,scale.z };
		root->name = scene->mRootNode->mName.C_Str();
		
		for (int i = 0; i < scene->mRootNode->mNumMeshes; ++i) //for each mesh in the root node
		{
			ComponentMesh* meshComp = (ComponentMesh*)root->CreateComponent(ComponentType::MESH); //create a new ComponentMesh and add it to the root node
			FillMeshData(meshComp, scene->mMeshes[scene->mRootNode->mMeshes[i]]); //Initialize that MeshComponent with the corresponding aiMesh
			meshComp->genBuffers();
			Debug.Log("New mesh loaded with %d vertex", meshComp->num_vertex);
		}
		
		RecursivelyHierarchy(scene->mRootNode, root, scene);

		aiReleaseImport(scene);
		App->renderer3D->CalculateSceneBoundingBox();
	}
	else
	{
		Debug.LogError("Error loading the scene %s. Error: %s", path, aiGetErrorString());
		ret = false;
	}
	return ret;
}

bool FBXLoader::RecursivelyHierarchy(const aiNode* parent, const GameObject* parentGO, const aiScene* scene)
{
	int i = 0;

	if(parent->mNumChildren > 0)
	for (aiNode* node = parent->mChildren[0]; i < parent->mNumChildren; ++i, node = parent->mChildren[i])
	{
		GameObject* newGO = App->scene->CreateGameObject((GameObject*)parentGO); //Generate a new GO for each child
		
		aiVector3D pos, scale;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scale, rotation, pos);
		newGO->transform->position = { pos.x, pos.y, pos.z };
		newGO->transform->rotation = { rotation.x,rotation.y,rotation.z,rotation.w };
		newGO->transform->scale = { scale.x,scale.y,scale.z };
		newGO->name = node->mName.C_Str();

		for (int j = 0; j < node->mNumMeshes; ++j) //Generate a new MeshComponent for each mesh in child
		{
			ComponentMesh* meshComp = (ComponentMesh*)newGO->CreateComponent(ComponentType::MESH);
			FillMeshData(meshComp, scene->mMeshes[node->mMeshes[j]]);
			meshComp->genBuffers();
			newGO->boundingBox.Enclose((float3*)meshComp->vertex, meshComp->num_vertex);
			newGO->createAABBbuffers();
			Debug.Log("New mesh loaded with %d vertex", meshComp->num_vertex);
		}
		RecursivelyHierarchy(node, newGO, scene); //Repeat it for each child
	}
	return true;
}

bool FBXLoader::FillMeshData(ComponentMesh* mymesh, aiMesh* mesh)
{
	mymesh->num_vertex = mesh->mNumVertices;
	mymesh->vertex = new float[mesh->mNumVertices * 3];
	memcpy(mymesh->vertex, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3); //Init my vertex with the vertices array

	if (mesh->HasFaces())
	{
		mymesh->num_index = mesh->mNumFaces * 3; //Assuming each face has 3 vertices => They all are triangles.
		mymesh->index = new uint[mymesh->num_index];
		for (uint j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
				Debug.LogWarning("Charging a geometry face with != 3 vertices! Some errors might happen");
			else // This face has 3 vertex

				memcpy(&mymesh->index[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint)); //copy the 3 index into the right place in our index array							
		}
	}

	if (mesh->HasNormals())
	{
		mymesh->normals = new float[mymesh->num_vertex * 3];
		memcpy(mymesh->normals, mesh->mNormals, sizeof(float) * mymesh->num_vertex * 3);
	}

	int colorChannels = mesh->GetNumColorChannels();
	if (colorChannels > 0)
	{
		for (int i = 0; i < colorChannels; ++i)
		{
			if (mesh->HasVertexColors(i))
			{
				mymesh->colors = new float[mymesh->num_vertex * 4];
				memcpy(mymesh->colors, mesh->mColors[i], sizeof(float) * mymesh->num_vertex * 4);
				break; //We only keep support of 1 set of colors, for now//
			}
		}
	}

	if (mesh->HasTextureCoords(0))
	{
		mymesh->textureCoords = new float[mymesh->num_vertex * 2];
		for (int j = 0; j < mymesh->num_vertex; ++j)
		{
			memcpy(&mymesh->textureCoords[j * 2], &mesh->mTextureCoords[0][j].x, sizeof(float));
			memcpy(&mymesh->textureCoords[(j * 2) + 1], &mesh->mTextureCoords[0][j].y, sizeof(float));
		}
	}	

	mymesh->name = mesh->mName.C_Str();

	//Save an own-formatted mesh file
	//SaveMesh(mymesh);

	return true;
}

bool FBXLoader::SaveMesh(ComponentMesh* mesh)
{
	uint ranges[5] =
	{
		mesh->num_vertex,
		mesh->num_index,
		mesh->colors ? mesh->num_vertex : 0,
		mesh->normals ? mesh->num_vertex : 0,
		mesh->textureCoords ? mesh->num_vertex : 0
	};

	uint fileSize = sizeof(ranges);
	fileSize += mesh->num_vertex * 3 * sizeof(float);
	fileSize += mesh->num_index * sizeof(uint);	
	if (mesh->colors)
	{
		fileSize += mesh->num_vertex * 4 * sizeof(float); //for each vertex 4 floats (rgba)
	}
	if (mesh->normals)
	{
		fileSize += mesh->num_vertex * 3 * sizeof(float); //for each vertex 3 floats (xyz)
	}
	if (mesh->textureCoords)
	{
		fileSize += mesh->num_vertex * 2 * sizeof(float); //for each vertex 2 floats (xy)
	}

	char* buffer = new char[fileSize];
	char* cursor = buffer; //Point to the beginning of the buffer

	//write ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes; //displace the writting point by bytes

	//write vertex
	bytes = mesh->num_vertex * 3 * sizeof(float);
	memcpy(cursor, mesh->vertex, bytes);
	cursor += bytes;

	//write index
	bytes = mesh->num_index * sizeof(uint);
	memcpy(cursor, mesh->index, bytes);
	cursor += bytes;

	//write colors
	if (mesh->colors)
	{
		bytes = mesh->num_vertex * sizeof(float) * 4;
		memcpy(cursor, mesh->colors, bytes);
		cursor += bytes;
	}

	//write normals
	if (mesh->normals)
	{
		bytes = mesh->num_vertex * sizeof(float) * 3;
		memcpy(cursor, mesh->normals, bytes);
		cursor += bytes;
	}

	//write textureCoords
	if (mesh->textureCoords)
	{
		bytes = mesh->num_vertex * sizeof(float) * 2;
		memcpy(cursor, mesh->textureCoords, bytes);
		cursor += bytes;
	}

	App->fs->OpenWriteBuffer("Library/" + mesh->name + ".jeje", buffer, fileSize);
	
	delete buffer;

	/*ComponentMesh* othermesh = new ComponentMesh(nullptr);
	LoadMesh(othermesh, "Library/" + mesh->name + ".jeje");*/

	return true;
}

bool FBXLoader::LoadMesh(ComponentMesh* mesh, std::string path)
{
	int fileSize = 0;
	char* buffer;
	App->fs->OpenRead(path, &buffer, fileSize);

	uint ranges[5];

	char* cursor = buffer;
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;
	
	mesh->num_vertex = ranges[0];
	mesh->num_index = ranges[1];

	bytes = mesh->num_vertex * sizeof(float) * 3;
	mesh->vertex = new float[mesh->num_vertex * 3];
	memcpy(mesh->vertex, cursor, bytes);
	cursor += bytes;

	bytes = mesh->num_index * sizeof(uint);
	mesh->index = new uint[mesh->num_index];
	memcpy(mesh->index, cursor, bytes);
	cursor += bytes;

	 //vertex index colors normals textures
	if (ranges[2] > 0)
	{
		bytes = mesh->num_vertex * sizeof(float) * 4;
		mesh->colors = new float[mesh->num_vertex * 4];
		memcpy(mesh->colors, cursor, bytes);
		cursor += bytes;
	}

	if (ranges[3] > 0)
	{
		bytes = mesh->num_vertex * sizeof(float) * 3;
		mesh->normals = new float[mesh->num_vertex * 3];
		memcpy(mesh->normals, cursor, bytes);
		cursor += bytes;
	}

	if (ranges[4] > 0)
	{
		bytes = mesh->num_vertex * sizeof(float) * 2;
		mesh->textureCoords = new float[mesh->num_vertex * 2];
		memcpy(mesh->textureCoords, cursor, bytes);
		cursor += bytes;
	}

	mesh->genBuffers();

	delete buffer;

	return true;
}
