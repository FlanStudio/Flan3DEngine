#include "ResourceTexture.h"

#include "Glew/include/glew.h"

ResourceTexture::~ResourceTexture()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}

bool ResourceTexture::LoadToMemory()
{
	//Create an OpenGL texture and initialize it with the active Image data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data_rgba);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool ResourceTexture::UnLoadFromMemory()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
		id = 0;
	}
	return true;
}

uint ResourceTexture::getBytes() const
{
	return sizeof(ResourceTexture);
}
