#include "Application.h"
#include "ModuleEditor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#include "imgui/tabs/imgui_tabs.h"
#include "imgui/imgui_stl.h"

#include "SDL/include/SDL.h"

//TEMP
#include "MathGeoLib_1.5/Geometry/Sphere.h"
#include "MathGeoLib_1.5/Geometry/AABB.h"
//ENDTEMP

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled) {}
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
			Debug.Log("Sphere spawned %d", 1111);
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

	if (logEnabled)
	{
		Debug.Draw("LogWindow", &logEnabled);
	}

	if (showConfig)
	{	
		ImGui::Begin("Configuration", &showConfig);

		if(ImGui::BeginMenu("options", true, true))
		{
			if (ImGui::MenuItem("Option1"))
			{
			}
			if (ImGui::MenuItem("Option2"))
			{
			}		
			ImGui::EndMenu();
		}

		if (ImGui::CollapsingHeader("Application"))
		{
			ImGuiInputTextFlags flags = 0;
			flags |= ImGuiInputTextFlags_EnterReturnsTrue;
			ImGui::InputText("\tEngine Name", &App->window->winTitle, flags);
			ImGui::InputText("\tOrganization", &App->window->organization, flags);
			ImGui::DragFloat("\tFPS", &ImGui::GetIO().Framerate);

		}
		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("You opened window");
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImGui::Text("You opened hardware");
		}

		ImGui::End();
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Quit"))
				return update_status::UPDATE_STOP;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			
			ImGui::MenuItem("Demo Window", "", &showdemowindow);
				
			ImGui::MenuItem("MGL window", "", &showMGLwindow);

			ImGui::MenuItem("LOG", "", &logEnabled);

			ImGui::MenuItem("Config", "", &showConfig);

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

void LogWindow::Log(const char* format, ...)
{
	int oldsize = NormalBuf.size();
	numLogs++;
	va_list args;
	va_start(args, format);
	NormalBuf.appendfv(format, args);
	NormalBuf.appendfv("\n", args);
	va_end(args);
	for (int new_size = oldsize; oldsize < new_size; oldsize++)
		if (NormalBuf[oldsize] == '\n')
			LineOffsets.push_back(oldsize);
	ScrollToBottom = true;
}
void LogWindow::LogWarning(const char* format, ...)
{
	numwarnings++;
	va_list args;
	va_start(args, format);
	WarningBuf.appendfv(format, args);
	WarningBuf.appendfv("\n", args);
	va_end(args);
	ScrollToBottom = true;
}
void LogWindow::LogError(const char* format, ...)
{
	numerrors++;
	va_list args;
	va_start(args, format);
	ErrorBuf.appendfv(format, args);
	ErrorBuf.appendfv("\n", args);
	va_end(args);
	ScrollToBottom = true;
}
void LogWindow::Draw(const char* title, bool* p_opened)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
	ImGui::Begin(title, p_opened);

	if (ImGui::Button("Log", ImVec2(80, 25)))
	{
		Log("Hay un caballo volador");
	}

	ImGui::SameLine();
	if (ImGui::Button("LogWarning", ImVec2(80, 25)))
	{
		LogWarning("Al caballo volador no le des drogas");
	}

	ImGui::SameLine();
	if (ImGui::Button("LogError", ImVec2(80, 25)))
	{
		LogError("Al caballo volador le has dado muchas drogas");
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear", ImVec2(80,25)))
	{
		Clear();
	}

	ImGui::BeginTabBar("");
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	logsTitle = std::string(std::string("Logs (") + std::to_string(numLogs) + std::string(")")).c_str();
	if (ImGui::AddTab(logsTitle.data()))
	{
		ImGui::TextUnformatted(NormalBuf.begin());
	}

	warningTitle = std::string(std::string("Warnings (") + std::to_string(numwarnings) + std::string(")")).c_str();
	if (ImGui::AddTab(warningTitle.data()))
	{
		ImGui::TextColored(ImVec4(255, 255, 0, 255), WarningBuf.begin());
	}

	errorTitle = std::string(std::string("Errors (") + std::to_string(numerrors) + std::string(")")).c_str();
	if (ImGui::AddTab(errorTitle.data()))
	{
		ImGui::TextColored(ImVec4(255, 0, 0, 255), ErrorBuf.begin());
	}
	ImGui::EndTabBar();
	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;

	ImGui::End();
}
