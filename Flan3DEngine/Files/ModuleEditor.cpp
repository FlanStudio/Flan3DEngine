#include "Application.h"
#include "ModuleEditor.h"

#include "MathGeoLib/MathGeoLib.h"

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

	//Editor setup down here

	if(showdemowindow)
		ImGui::ShowDemoWindow(&showdemowindow);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Toggle demo window"))
				showdemowindow = !showdemowindow;
			if (ImGui::MenuItem("Quit"))
				return update_status::UPDATE_STOP;
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	int i = 0;
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_STATE::KEY_DOWN)
		//Random here?
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	/*SDL_GL_SwapWindow(App->window->window);*/
	ImGui::EndFrame();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	return true;
}