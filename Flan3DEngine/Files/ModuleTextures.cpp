#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleFileSystem.h"


#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")


ModuleTextures::~ModuleTextures()
{
	deleteTextures();
}

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
	
	return true;
}

uint ModuleTextures::LoadTexture(char* file, bool useFileSystem)
{
	//TODO: MORE TEXTURES SUPPORT?
	deleteTextures();

	char* buffer = nullptr;
	int size;

	if(useFileSystem)
		if (!App->fs->OpenRead(file, &buffer, size)) //Get the image compressed in a buffer
		{
			return 0;
		}
	
	ILuint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	if (useFileSystem)
	{
		if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size)) //Uncompress the image stored in the buffer into the active Image
		{
			Debug.LogError("Error loading the image \"%s\". Error: %s", file, iluErrorString(ilGetError()));
			return 0;
		}
	}
	else
	{
		if(!ilLoad(IL_TYPE_UNKNOWN, file))
		{
			Debug.LogError("Error loading the image \"%s\". Error: %s", file, iluErrorString(ilGetError()));
			return 0;
		}
	}
	

	if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) //Convert the active image into rgba format
	{
		Debug.LogError("Error converting the image \"%s\" to RGBA. Error: %s", file, iluErrorString(ilGetError()));
		return 0;
	}

	if (!iluFlipImage())
	{
		Debug.LogError("Error flipping the image \"%s\". Error: %s", file, iluErrorString(ilGetError()));
		return 0;
	}

	//Create an OpenGL texture and initialize it with the active Image data
	uint ImageName = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ImageName);
	glBindTexture(GL_TEXTURE_2D, ImageName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glBindTexture(GL_TEXTURE_2D, 0);

	//Create our local storage of the texture
	Texture* text = new Texture;
	text->data = ilGetData();
	text->id = ImageName; //THE OPENGL ID, NOT THE IMAGE ONE!
	text->width = ilGetInteger(IL_IMAGE_WIDTH);
	text->height = ilGetInteger(IL_IMAGE_HEIGHT);
	textures.push_back(text);

	//delete the buffer obtained from fileSystem and the unnecessary Image
	if(buffer)
		delete buffer;
	ilDeleteImage(id);

	//returns the OpenGL id for that texture
	return ImageName;
}

void ModuleTextures::getTextureSize(uint id, uint& width, uint& height) const
{
	if (id < textures.size())
	{		
		width = textures[id]->width;
		height = textures[id]->height;
	}
	else
	{
		Debug.LogError("Texture not contained in the textures vector!");
	}
}

void ModuleTextures::deleteTextures()
{
	for (int i = 0; i < textures.size(); ++i)
	{
		glDeleteTextures(1, &textures[i]->id);
	}
	textures.clear();
}