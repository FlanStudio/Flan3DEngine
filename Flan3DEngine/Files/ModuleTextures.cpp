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
	LoadTexture("Assets/test.png");
	return true;
}

void ModuleTextures::LoadTexture(char* file)
{
	/*char* buffer;
	int size;
	App->fs->OpenRead(file, &buffer, size);
	
	uint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id),
	ilLoadL(IL_TYPE_UNKNOWN, buffer, size);

	ILubyte* data = ilGetData();


	delete buffer;*/
}