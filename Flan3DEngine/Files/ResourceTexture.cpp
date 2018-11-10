#include "ResourceTexture.h"

#include "Glew/include/glew.h"

ResourceTexture::~ResourceTexture()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}

uint ResourceTexture::getBytes() const
{
	return sizeof(ResourceTexture);
}
