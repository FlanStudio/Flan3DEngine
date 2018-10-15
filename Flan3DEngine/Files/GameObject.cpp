#include "GameObject.h"


Component* GameObject::CreateComponent(ComponentType type)
{
	Component* ret = nullptr;

	switch (type)
	{
		case ComponentType::MESH:
		{
			//TODO: App renderer create component mesh, and save the pointer returned into our vector
			break;
		}
	}
	return ret;
}
