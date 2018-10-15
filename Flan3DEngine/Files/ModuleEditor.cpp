#include "Application.h"
#include "ModuleEditor.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#include "imgui/tabs/imgui_tabs.h"
#include "imgui/imgui_stl.h"

#include "SDL/include/SDL.h"

#include "ModuleFileSystem.h"

#include "Brofiler\Brofiler.h"

#include "assimp/include/version.h"
#include "DevIL/include/il.h"


//TEMP
#include "MathGeoLib_1.5/Geometry/Sphere.h"
#include "MathGeoLib_1.5/Geometry/AABB.h"
//ENDTEMP

ModuleEditor::ModuleEditor(bool start_enabled) : Module("ModuleEditor", start_enabled) {}
ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Init()
{
	return true;
}

bool ModuleEditor::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	ImGui::StyleColorsDark();

	return true;	
}

update_status ModuleEditor::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleEditor_Preupdate", Profiler::Color::AntiqueWhite)

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
			Sphere sp;
			sp.pos = { 0,1,0 };
			sp.r = 1;

			AABB spBB = AABB(sp);
			somethingiscolliding = false;
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Spawn 2 Spheres at 0,0", ImVec2(170, 50)))
		{
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

		ImGui::Text(somethingiscolliding ? "true" : "false");
		
		ImGui::End();
	}

	if (showConfig)
	{	
		ImGui::Begin("Configuration", &showConfig);

		if(ImGui::BeginMenu("options", true, true))
		{
			if (ImGui::MenuItem("SaveConfig"))
			{
				App->Save();
			}
			if (ImGui::MenuItem("LoadConfig"))
			{
				App->Load();
			}		
			ImGui::EndMenu();
		}
		

		if (ImGui::CollapsingHeader("Application"))
		{
			ImGuiInputTextFlags flags = 0;
			flags |= ImGuiInputTextFlags_EnterReturnsTrue;
			ImGui::InputText("\tEngine Name", &App->engineName, flags);
			ImGui::InputText("\tOrganization", &App->organization, flags);	
			ImGui::SliderInt("\tMax FPS", &App->maxFPS, 0, 125);

			char FPSTitle[50];
			sprintf(FPSTitle, "FPS %.1f", App->FPS[App->FPS_index]);
			ImGui::PlotHistogram("", App->FPS.data(), App->FPS.size(), App->FPS_index, FPSTitle, 0, 120, ImVec2(ImGui::GetWindowWidth() * 0.65, 250));
						
			char msTitle[50];
			sprintf(msTitle, "ms %.2f", App->ms[App->ms_index]);
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6);
			ImGui::PlotHistogram("", App->ms.data(), App->ms.size(), App->ms_index, msTitle, 0, 30, ImVec2(ImGui::GetWindowWidth() * 0.65, 75));

		}

		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("Refresh Rate: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", App->window->getRefreshRate());
			ImGui::NewLine();

			if (ImGui::Checkbox("FullScreen\t", &WIN_FULLSCREEN))
			{
				App->window->setFullScreenWindow(WIN_FULLSCREEN);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("FullScreen Desktop", &WIN_FULLSCREEN_DESKTOP))
			{
				App->window->setFullScreenDesktop(WIN_FULLSCREEN_DESKTOP);
			}

			if (ImGui::SliderFloat("Brightness", &BRIGHTNESS, 0.0, 1.0, "%.2f"))
			{
				App->window->setBrightness(BRIGHTNESS);
			}

			if (ImGui::SliderInt("Width", &SCREEN_WIDTH, 0, 1920))
			{
				App->window->setWindowWidth(SCREEN_WIDTH);
			}

			if (ImGui::SliderInt("Height", &SCREEN_HEIGHT, 0, 1080))
			{
				App->window->setWindowHeight(SCREEN_HEIGHT);
			}

			if (ImGui::Checkbox("Borderless\t", &WIN_BORDERLESS))
			{
				App->window->setBorderless(WIN_BORDERLESS);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &WIN_RESIZABLE))
			{
				App->window->setResizable(WIN_RESIZABLE);
			}
		}

		if (ImGui::CollapsingHeader("Renderer"))
		{
			App->renderer3D->guiRenderer();
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			SDL_version version;
			SDL_GetVersion(&version);
			ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d.%d.%d", version.major, version.minor, version.patch);
			ImGui::Separator();
			ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
			ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%.1fGb", ((float)SDL_GetSystemRAM())*0.001);
			
			ImGui::Text("Caps: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%s %s %s %s %s %s %s %s %s %s %s",
															SDL_HasRDTSC() ? "RDTSC" : "", SDL_HasMMX() ? "MMX" : "", SDL_HasSSE() ? "SSE" : "", SDL_HasSSE2() ? "SSE2" : "",
															SDL_HasSSE3() ? "SSE3" : "", SDL_HasSSE41() ? "SSE41" : "", SDL_HasSSE42() ? "SSE42" : "", SDL_HasAVX() ? "AVX" : "",
															SDL_HasAVX2() ? "AVX2" : "", SDL_HasAltiVec() ? "AltiVec" : "", SDL_Has3DNow() ? "3DNOW" : "");

			ImGui::Separator();

			App->renderer3D->guiGPU();
			ImGui::Text("VRAM Budget: ");
			ImGui::Text("VRAM Usage: ");
			ImGui::Text("VRAM Available: ");
			ImGui::Text("VRAM Reserved: ");

		}

		ImGui::End();
	}
	
	if (logEnabled)
	{
		Debug.Draw("LogWindow", &logEnabled);
	}

	if (showAbout)
	{
		ImGui::Begin("About", &showAbout);
		
		if (ImGui::CollapsingHeader("About Us"))
		{
			ImGui::Text("%s\n", App->engineName.data());
			if (ImGui::Button("Download Latest"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/FlanStudio/Flan3DEngine/releases", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::Button("Report a Bug   ##b2"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/FlanStudio/Flan3DEngine/issues", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::Button("Documentation  ##b3"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/FlanStudio/Flan3DEngine", NULL, NULL, SW_SHOWNORMAL);
			}
		}

		if (ImGui::CollapsingHeader("Description"))
		{
			ImGui::Text("%s is a Game Engine for producing awesome videogames.", App->engineName.data());
		}

		if (ImGui::CollapsingHeader("Authors"))
		{
			ImGui::Text("Jonathan Molina-Prados\t");
			ImGui::SameLine();
			if (ImGui::Button("Link to Github"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/Jony635", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("Oriol de Dios \t\t\t");
			ImGui::SameLine();
			if (ImGui::Button("Link to Github##b2"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/orioldedios", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		if (ImGui::CollapsingHeader("Libraries"))
		{
			ImGui::Text("ImGui  "); ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 },"%s \t", IMGUI_VERSION);
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b2"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("\tMod: ImGui_Tabs  ");
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b21"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/scottmudge/imgui_tabs", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("MathGeoLib"); ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 }," 1.5  \t");
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b3"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("pcg c basic"); ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 }, " 0.9 \t");
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b4"))
			{
				ShellExecuteA(NULL, "Open", "http://www.pcg-random.org/", NULL, NULL, SW_SHOWNORMAL);
			}

			SDL_version version;
			SDL_GetVersion(&version);
			ImGui::Text("SDL  "); ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 }, "%d.%d.%d  \t\t", version.major, version.minor, version.patch);
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b5"))
			{
				ShellExecuteA(NULL, "Open", "https://www.libsdl.org/download-2.0.php", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("Assimp "); ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 }, "%i.%i.%i\t\t", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b6"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/assimp/assimp", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Text("DevIL");ImGui::SameLine();
			ImGui::TextColored({ 1,1,0,1 }, "  %i.%i.%i\t\t", IL_VERSION/100, IL_VERSION / 10 - ((IL_VERSION / 100) * 10), IL_VERSION % 10);
			ImGui::SameLine();
			if (ImGui::Button("Link to Library##b7"))
			{
				ShellExecuteA(NULL, "Open", "https://github.com/DentonW/DevIL", NULL, NULL, SW_SHOWNORMAL);
			}

		}

		if (ImGui::CollapsingHeader("License"))
		{
			ImGui::Text("Copyright (c) 2018 FlanStudio\t");
			ImGui::SameLine();
			if (ImGui::Button("MIT License"))
			{
				ShellExecuteA(NULL, "Open", "https://opensource.org/licenses/MIT", NULL, NULL, SW_SHOWNORMAL);
			}
		}

		ImGui::End();
	}

	if (propWindow)
	{
		ImGui::Begin("Properties",&propWindow);

		if (ImGui::CollapsingHeader("Transformation"))
		{
			ImGui::TextColored({ 1,1,0,1 }, "All these properties are not visible in the mesh."); ImGui::Separator();
			App->scene->guiMeshesTransform();
		}

		if (ImGui::CollapsingHeader("Geometry"))
		{
			App->renderer3D->guiMeshesGeometry();
		}

		if (ImGui::CollapsingHeader("Textures"))
		{
			App->textures->guiTextures();
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

			ImGui::MenuItem("Properties", "", &propWindow);

			ImGui::MenuItem("About", "", &showAbout);

			ImGui::EndMenu();
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextColored({ 1,1,1,1 }, "Press ESC to close all windows opened");
			ImGui::EndTooltip();
		}
		
		ImGui::EndMainMenuBar();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		showdemowindow = false;
		showMGLwindow = false;
		somethingiscolliding = false;
		logEnabled = false;
		showConfig = false;
		showAbout = false;
		propWindow = false;
	}
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

void LogWindow::Draw(const char* title, bool* p_opened)
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
	ImGui::Begin(title, p_opened);

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

void ModuleEditor::Draw() const
{
	BROFILER_CATEGORY("ModuleEditor_Postupdate", Profiler::Color::AntiqueWhite)

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();

}
