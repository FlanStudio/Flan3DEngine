#include "ModuleEditor.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Init()
{
	return true;
}

bool ModuleEditor::Start()
{
	SDL_GL_CreateContext(window);

	return true;	
}

update_status ModuleEditor::PreUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	return true;
}

void ModuleEditor::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{}