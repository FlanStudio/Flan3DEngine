#ifndef __MODULERENDERER3D_H__
#define __MODULERENDERER3D_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "MathGeoLib_1.5/Math/float4x4.h"
#include "MathGeoLib_1.5/Geometry/AABB.h"
#include <vector>


#define MAX_LIGHTS 8

class ComponentMesh;
class GameObject;
class ComponentCamera;

class ModuleRenderer3D : public Module
{
	friend class ComponentMesh;
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const;

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj);

	void OnResize(int width, int height);

	void guiGPU()const;
	void guiRenderer();
	void guiMeshesGeometry() const;

	void setWireframe(bool boolean);

	void AddMesh(ComponentMesh* newMesh) { meshes.push_back(newMesh); }
	void ClearMeshes();
	void ClearMesh(ComponentMesh* mesh);
	void UpdateNormalsLenght();
	void DrawMeshes() const;

	void CalculateSceneBoundingBox();
	AABB getSceneAABB() const { return sceneBoundingBox; }


	ComponentMesh* CreateComponentMesh(GameObject* parent);

public:
	void ReserveMeshes(uint size) 
	{ 
		meshes.reserve(size); 
	}
	
public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	AABB sceneBoundingBox; //A bounding box containing all the geometries //TODO: change that system

private:
	bool depthTest = false, cullFace = false, lighting = false, colorMaterial = false, texture2D = false;
	bool wireframe = false;

	std::vector<ComponentMesh*> meshes; //Only contain meshes for now

	bool drawNormals = false;
	float normalsLenght = 0.5f;
};
#endif