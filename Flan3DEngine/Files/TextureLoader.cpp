#include "TextureLoader.h"
#include "Application.h"


#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

bool TextureLoader::Init()
{
	ilInit();
	ILuint devilError = ilGetError();

	if (devilError != IL_NO_ERROR) 
	{
		Debug.LogError("Error initializing DevIL! Error: %s", iluErrorString(devilError));
	}

	return true;
}

void TextureLoader::LoadTexture(char* file)
{
	ILenum type;
}