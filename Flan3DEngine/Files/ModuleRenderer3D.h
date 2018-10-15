#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "MathGeoLib_1.5/Math/float4x4.h"
#include "MathGeoLib_1.5/Geometry/AABB.h"
#include <vector>

#define MAX_LIGHTS 8

struct Mesh;

class ModuleRenderer3D : public Module
{
	friend class Mesh;
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	//Save changes the JSON, not the module
	bool Save(JSON_Object* obj) const;

	//Load changes the module, not the JSON
	bool Load(const JSON_Object* obj);

	void OnResize(int width, int height);

	void guiGPU()const;
	void guiRenderer();
	void guiMeshesTransform() const;
	void guiMeshesGeometry() const;

	void setWireframe(bool boolean);

	void AddMeshes(std::vector<Mesh*> newMeshes);
	void ClearMeshes();
	void UpdateNormalsLenght();
	void DrawMeshes() const;

	void CalculateSceneBoundingBox();
	AABB getSceneAABB() const { return sceneBoundingBox; }

private:
	float4x4 Perspective(float fovy, float aspect, float znear, float zfar);
	
public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	AABB sceneBoundingBox; //A bounding box containing all the geometries

private:
	bool depthTest = false, cullFace = false, lighting = false, colorMaterial = false, texture2D = false;
	bool wireframe = false;
	float4x4 ProjectionMatrix;
	//TODO: TWO OTHERS

	std::vector<Mesh*> meshes;
	bool drawNormals = false;
	float normalsLenght = 0.5f;
	
};