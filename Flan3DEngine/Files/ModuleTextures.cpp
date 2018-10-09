#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleFileSystem.h"


#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

bool ModuleTextures::Init()
{
	ILuint devilError = 0;

	ilInit();
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
	{
		Debug.LogError("TEXTURE: Error initializing IL sublibrary. Error: %s", iluErrorString(devilError));
		return false;
	}

	iluInit();
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
	{
		Debug.LogError("TEXTURE: Error initializing ILU sublibrary. Error: %s", iluErrorString(devilError));
		return false;
	}

	ilutRenderer(ILUT_OPENGL);
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
	{
		Debug.LogError("TEXTURE: Error initializing ILUT sublibrary. Error: %s", iluErrorString(devilError));
		return false;
	}

	return true;
}

bool ModuleTextures::Start()
{
	LoadTexture("Assets/test.jpg");
	return true;
}

void ModuleTextures::LoadTexture(char* file)
{
	char* buffer;
	int size;
	App->fs->OpenRead(file, &buffer, size);
	
	ILuint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);
	ilLoadL(IL_TYPE_UNKNOWN, buffer, size);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	Texture* text = new Texture;
	text->data = ilGetData();
	text->id = id;
	textures.push_back(text);

	


	delete buffer;
}

void ModuleTextures::getTextureSize(uint id, uint& width, uint& height)
{
	if (id < textures.size())
	{
		ilBindImage(textures[id]->id);
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		ilBindImage(0);
	}
	else
	{
		//TODO: ERROR HERE
	}
}