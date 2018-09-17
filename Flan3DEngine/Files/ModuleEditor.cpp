#include "Application.h"
#include "ModuleEditor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "SDL/include/SDL.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Init()
{
	return true;
}

bool ModuleEditor::Start()
{
	SDL_GLContext gl_context = SDL_GL_CreateContext(App->window->window);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	ImGui_ImplOpenGL2_Init();

	ImGui::StyleColorsDark();
	return true;	
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(&showdemowindow);


	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGui::Render();
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	/*SDL_GL_SwapWindow(App->window->window);*/
	ImGui::EndFrame();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	return true;
}

void ModuleEditor::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{}