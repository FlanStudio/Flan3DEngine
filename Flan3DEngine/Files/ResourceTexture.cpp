#include "ResourceTexture.h"

#include "Glew/include/glew.h"

ResourceTexture::~ResourceTexture()
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}

	if (data_rgba)
	{
		delete data_rgba;
		data_rgba = nullptr;
	}
}

void ResourceTexture::SerializeToMeta(char*& cursor) const
{
	uint bytes = sizeof(UID);
	memcpy(cursor, &uuid, bytes);
	cursor += bytes;

	bytes = sizeof(uint);
	memcpy(cursor, &width, bytes);
	cursor += bytes;

	memcpy(cursor, &height, bytes);
	cursor += bytes;

	bytes = width * height * 4;
	memcpy(cursor, data_rgba, bytes);
	cursor += bytes;
}

void ResourceTexture::DeSerializeFromMeta(char*& cursor)
{
	uint bytes = sizeof(UID);
	memcpy(&uuid, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(uint);
	memcpy(&width, cursor, bytes);
	cursor += bytes;

	memcpy(&height, cursor, bytes);
	cursor += bytes;

	bytes = width * height * 4;

	if (data_rgba)
		delete data_rgba;

	data_rgba = new unsigned char[bytes];
	memcpy(data_rgba, cursor, bytes);
	cursor += bytes;
}

uint ResourceTexture::bytesToSerializeMeta() const
{
	return sizeof(UID) + sizeof(uint) * 2 + width * height * 4;
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
