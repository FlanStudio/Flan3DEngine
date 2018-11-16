#include "Resource.h"

void Resource::Referenced()
{
	if (timesReferenced == 0)
		LoadToMemory();

	timesReferenced++;
}

void Resource::deReferenced()
{
	if (timesReferenced == 1)
		UnLoadFromMemory();

	timesReferenced--;
}
