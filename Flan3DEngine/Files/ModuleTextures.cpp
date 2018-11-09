#include "ModuleTextures.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "DevIL/include/il.h"
#include "DevIL/include/ilu.h"
#include "DevIL/include/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "ResourceTexture.h"

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
		delete textures[i];
	}
	textures.clear();
}

void ModuleTextures::guiTextures() const
{
	for (int i = 0; i < textures.size(); ++i)
	{
		ImGui::Text("Texture %i", i); ImGui::NewLine();
		ImGui::SetCursorPosX(36);
		ImGui::Image((GLuint*)textures[i]->id, { 50,50 }, { 0,1 }, {1,0});
		ImGui::Text("\tsize: %dx%d", textures[i]->width, textures[i]->height);
	}
}

bool ModuleTextures::isSupported(const std::string& extension) const
{
	if (extension == ".bmp" || extension == ".BMP"	||
		extension == ".cut" || extension == ".CUT"	||
		extension == ".dds" || extension == ".DDS"	||
		extension == ".gif"	|| extension == ".GIF"	||
		extension == ".ico" || extension == ".ICO"	||
		extension == ".cur"	|| extension == ".CUR"	||
		extension == ".jpg" || extension == ".JPG"	||
		extension == ".jpe"	|| extension == ".JPE"	||
		extension == ".jpeg"|| extension == ".JPEG" ||
		extension == ".lbm"	|| extension == ".LBM"	||
		extension == ".lif" || extension == ".LIF"	||
		extension == ".lmp" || extension == ".LMP"	||
		extension == ".mdl" || extension == ".MDL"	||
		extension == ".mng" || extension == ".MNG"	||
		extension == ".pcd" || extension == ".PCD"	||
		extension == ".pcx" || extension == ".PCX"	||
		extension == ".pic" || extension == ".PIC"	||
		extension == ".png" || extension == ".PNG"	||
		extension == ".pbm" || extension == ".PBM"	||
		extension == ".pgm" || extension == ".PGM"	||
		extension == ".ppm" || extension == ".PPM"	||
		extension == ".pnm" || extension == ".PNM"	||
		extension == ".psd" || extension == ".PSD"	||
		extension == ".sgi" || extension == ".SGI"	||
		extension == ".bw"  || extension == ".BW"	||
		extension == ".rgb" || extension == ".RGB"	||
		extension == ".rgba"|| extension == ".RGBA"	||
		extension == ".tga" || extension == ".TGA"	||
		extension == ".tif" || extension == ".TIF"	||
		extension == ".tiff"|| extension == ".TIFF"	||
		extension == ".wal"|| extension == ".WAL")
			return true;
	
	return false;
}

ResourceTexture* ModuleTextures::ExportResource(std::string file)
{
	//Get the buffer from the FileSystem
	char* buffer;
	int size;
	if (!App->fs->OpenRead(file, &buffer, size))
	{
		return nullptr;
	}

	//Create and load an empty DevIL image
	ILuint ilID;
	ilGenImages(1, &ilID);
	ilBindImage(ilID);

	//Load the current binded image with the buffer received from the FileSystem
	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		Debug.LogError("Error loading the image \"%s\". Error: %s", file, iluErrorString(ilGetError()));
		return nullptr;
	}

	if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) //Convert the active image into rgba format
	{
		Debug.LogError("Error converting the image \"%s\" to RGBA. Error: %s", file, iluErrorString(ilGetError()));
		return nullptr;
	}

	if (!iluFlipImage())
	{
		Debug.LogError("Error flipping the image \"%s\". Error: %s", file, iluErrorString(ilGetError()));
		return nullptr;
	}
	
	//Create an OpenGL texture and initialize it with the active Image data
	uint openGLID = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &openGLID);
	glBindTexture(GL_TEXTURE_2D, openGLID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glBindTexture(GL_TEXTURE_2D, 0);

	//Create our local storage of the texture
	ResourceTexture* text = new ResourceTexture();
	text->id = openGLID;
	text->width = ilGetInteger(IL_IMAGE_WIDTH);
	text->height = ilGetInteger(IL_IMAGE_HEIGHT);
	text->setFile((char*) file.data());

	textures.push_back(text);
	
	ILuint savedSize;
	ILubyte* savedBuffer = nullptr;

	std::string resourcePath = TEXTURES_LIBRARY_FOLDER + std::string("/") + std::to_string(text->getUUID()) + ".dds";

	if (App->fs->getExt(file) != ".dds")
	{
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

		savedSize = ilSaveL(IL_DDS, NULL, 0);
		savedBuffer = new ILubyte[savedSize];
		if (ilSaveL(IL_DDS, savedBuffer, savedSize) <= 0)
		{
			Debug.LogError("DevIL failed saving the texture \"%s\" to a buffer. Error: %s", file.data(), iluErrorString(ilGetError()));
			return nullptr;
		}

		if (!App->fs->OpenWriteBuffer(resourcePath, savedBuffer, savedSize))
		{
			return nullptr;
		}
	}
	else
	{
		if (!App->fs->OpenWriteBuffer(resourcePath, buffer, size))
		{
			return nullptr;
		}
	}

	text->setExportedFile((char*)resourcePath.data());

	//delete the buffer obtained from fileSystem and the unnecessary DevIL image
	if (buffer)
		delete buffer;

	if (savedBuffer)
		delete savedBuffer;

	ilDeleteImage(ilID);

	return text;
}
