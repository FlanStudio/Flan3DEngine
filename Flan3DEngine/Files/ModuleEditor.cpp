#include "Application.h"
#include "ModuleEditor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

//TEMP
#include "MathGeoLib_1.5/Geometry/Sphere.h"
#include "MathGeoLib_1.5/Geometry/AABB.h"

//ENDTEMP



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

	if (showMGLwindow)
	{
		ImGui::Begin("MathGeoLib Info", &showMGLwindow);

		if (ImGui::Button("Spawn 1 Sphere at 0,0", ImVec2(170, 50)))
		{
			//TODO: Spawn sphere
			Sphere sp;
			sp.pos = { 0,1,0 };
			sp.r = 1;

			AABB spBB = AABB(sp);
			somethingiscolliding = false;
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Spawn 2 Spheres at 0,0", ImVec2(170, 50)))
		{
			//TODO: Spawn sphere
			Sphere sp;
			sp.pos = { 0,1,0 };
			sp.r = 1;

			AABB spBB = AABB(sp);
			
			Sphere sp2;
			sp2.pos = { 0,1,0 };
			sp2.r = 1;

			AABB sp2BB = AABB(sp2);		
			
			somethingiscolliding = spBB.Intersects(sp2BB);
		}

		ImGui::NewLine();
		ImGui::Text("Something is colliding?:");
		ImGui::SameLine();

		//TODO: CHECKCOLLISIONS
		ImGui::Text(somethingiscolliding ? "true" : "false");
		
		ImGui::End();
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Toggle demo window"))
				showdemowindow = !showdemowindow;
			if (ImGui::MenuItem("Toggle MGL window"))
				showMGLwindow = !showMGLwindow;
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