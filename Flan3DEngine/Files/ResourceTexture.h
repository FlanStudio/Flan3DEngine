#ifndef __RESOURCETEXTURE_H__
#define __RESOURCETEXTURE_H__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture() : Resource(ResourceType::TEXTURE) {}
	virtual ~ResourceTexture();

	//OpenGL id
	uint id = 0;

	uint width = 0;
	uint height = 0;

private:
	uint getBytes() const;
};

#endif