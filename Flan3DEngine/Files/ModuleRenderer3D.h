#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
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

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

private:
	bool depthTest = false, cullFace = false, lighting = false, colorMaterial = false, texture2D = false;
	bool wireframe = false;
	//TODO: TWO OTHERS
};