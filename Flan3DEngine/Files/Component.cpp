#include "Component.h"

char* ComponentType_toString(ComponentType type)
{
	switch (type)
	{
	case ComponentType::TRANSFORM:
		return "Transform";
	case ComponentType::MESH:
		return "Mesh";
	case ComponentType::MATERIAL:
		return "Material";
	case ComponentType::CAMERA:
		return "Camera";
	case ComponentType::SCRIPT:
		return "Script";
	default:
		return nullptr;
	}
}
