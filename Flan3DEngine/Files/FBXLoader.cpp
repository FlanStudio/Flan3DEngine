#include "FBXLoader.h"
#include "Application.h"

#include "assimp/include/scene.h"
#include "assimp/include/postprocess.h"
#include "assimp/include/cfileio.h"

#include <string>

#pragma comment (lib, "Assimp/libx86/assimp-vc140-mt.lib")


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
	meshes.reserve(maxMeshes);

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
	for (int i = 0; i < meshes.size(); ++i)
	{		
		meshes[i]->Draw();

		if(drawNormals)
			meshes[i]->drawNormals();
		
	}
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
	bool ret = false;
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
		clearMeshes();
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			Mesh* mymesh = new Mesh();
			aiMesh* mesh = scene->mMeshes[i];
			
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
				mymesh->normals = new float[mymesh->num_index * 3];
				memcpy(mymesh->normals, mesh->mNormals, sizeof(float) * mymesh->num_index * 3);
			}
			
			int colorChannels = mesh->GetNumColorChannels();
			if (colorChannels > 0)
			{
				for (int i = 0; i < colorChannels; ++i)
				{
					if (mesh->HasVertexColors(i))
					{
						mymesh->colors = new float[mymesh->num_index * 4];
						memcpy(mymesh->colors, mesh->mColors[i], sizeof(float) * mymesh->num_index * 4);
						break; //We only keep support of 1 set of colors, for now
					}
				}
			}

			int uvChannels = mesh->GetNumUVChannels();
			{
				if (uvChannels > 0)
				{
					for (int i = 0; i < uvChannels; ++i)
					{
						if (mesh->HasTextureCoords(i))
						{
							if (mesh->mNumUVComponents[i] == 2)	//We only support plain textures for now
							{
								mymesh->textureCoords = new float[mymesh->num_index * 3]; //The original array comes as x,y,z.
								memcpy(mymesh->textureCoords, mesh->mTextureCoords[i], sizeof(mymesh->num_index * 3));
								break; //Only 1 texture coord per vertex for now
							}							
						}
					}
				}
			}

			mymesh->genBuffers();			
			meshes.push_back(mymesh);
			Debug.Log("New mesh loaded with %d vertices", mymesh->num_vertex);
		}
		aiReleaseImport(scene);

	}
	else
	{
		Debug.LogError("Error loading the scene %s. Error: %s", path, aiGetErrorString());
		ret = false;
	}
	return ret;
}

void FBXLoader::deleteFBX(Mesh* mesh)
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		if (meshes[i] == mesh)
		{
			delete mesh;
			meshes.erase(meshes.begin() + i);
		}
	}
}

void FBXLoader::clearMeshes()
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		delete meshes[i];
	}
	std::vector<Mesh*>().swap(meshes); //Clear, but deleting the preallocated memory
}

void FBXLoader::UpdateNormalsLenght()
{
	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->UpdateNormalsLenght();
	}
}

//-----------------Mesh methods--------------------------

Mesh::~Mesh()
{ 
	destroyBuffers(); 
	delete[] normals;
	delete[] vertex;
	delete[] index;
	delete[] normalLines;
	delete[] colors;
	normals = vertex = normalLines = colors = nullptr;
	index = nullptr;
}

void Mesh::genBuffers()
{
	glGenBuffers(1, &vertex_ID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &index_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_index, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	if (normals)
	{
		glGenBuffers(1, &normals_ID);
		glBindBuffer(GL_ARRAY_BUFFER, normals_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_index * 3, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
	
		genNormalLines();
		glGenBuffers(1, &normalLines_ID);
		glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_index * 3 * 2, normalLines, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
	}

	if (colors)
	{
		glGenBuffers(1, &colors_ID);
		glBindBuffer(GL_ARRAY_BUFFER, colors_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_index * 4, colors, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (textureCoords)
	{
		glGenBuffers(1, &textureCoords_ID);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoords_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_index * 3, textureCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Mesh::destroyBuffers()
{
	glDeleteBuffers(1, &vertex_ID);
	glDeleteBuffers(1, &index_ID);
	glDeleteBuffers(1, &normals_ID);
	glDeleteBuffers(1, &normalLines_ID);
	glDeleteBuffers(1, &colors_ID);
	glDeleteBuffers(1, &textureCoords_ID);
	vertex_ID = index_ID = normals_ID = normalLines_ID = colors_ID = textureCoords_ID = 0;
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_ID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normals_ID);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colors_ID);
	glColorPointer(4, GL_FLOAT, 0, NULL);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, textureCoords_ID);
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_ID);
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::genNormalLines()
{	
	normalLines = new float[num_index * 3 * 2];
	for (int i = 0; i < num_index; i++)
	{		
		//origin coordinates
		normalLines[i*6]		=	vertex[index[i]*3];									//x
		normalLines[i*6+1]		=	vertex[index[i]*3 + 1 ];							//y
		normalLines[i*6+2]		=	vertex[index[i]*3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3]	=	normals[i*3] * App->fbxLoader->normalsLenght + normalLines[i*6];				//x
		normalLines[i * 6 + 4]	=	normals[i * 3 + 1] * App->fbxLoader->normalsLenght + normalLines[i*6 + 1];		//y
		normalLines[i * 6 + 5]	=	normals[i * 3 + 2] * App->fbxLoader->normalsLenght + normalLines[i*6 + 2];		//z
	}
}

void Mesh::UpdateNormalsLenght()
{
	if (!normalLines)
		return;

	for (int i = 0; i < num_index; i++)
	{
		//origin coordinates
		normalLines[i * 6] = vertex[index[i] * 3];									//x
		normalLines[i * 6 + 1] = vertex[index[i] * 3 + 1];							//y
		normalLines[i * 6 + 2] = vertex[index[i] * 3 + 2];								//z

		//destination coordinates
		normalLines[i * 6 + 3] = normals[i * 3] * App->fbxLoader->normalsLenght + normalLines[i * 6];				//x
		normalLines[i * 6 + 4] = normals[i * 3 + 1] * App->fbxLoader->normalsLenght + normalLines[i * 6 + 1];		//y
		normalLines[i * 6 + 5] = normals[i * 3 + 2] * App->fbxLoader->normalsLenght + normalLines[i * 6 + 2];		//z
	}

	glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_index * 3 * 2, normalLines, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::drawNormals()
{
	if (normalLines)
	{
		glColor3f(1, 0, 0);

		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, normalLines_ID);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, num_index * 2);

		glDisableClientState(GL_VERTEX_ARRAY);

		glColor3f(1, 1, 1);
	}	
}