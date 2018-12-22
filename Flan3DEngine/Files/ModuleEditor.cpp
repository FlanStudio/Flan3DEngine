#include "Application.h"
#include "ModuleEditor.h"
#include "Globals.h"
#include "../Game/ColorTextEditor/ImGuiColorTextEdit/ImGuiColorTextEdit/TextEditor.h"
#include "../Game/ColorTextEditor/imp.h"

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

#include "imgui/ImGuizmo/ImGuizmo.h"
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

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

	customStyle();

	return true;	
}

update_status ModuleEditor::PreUpdate()
{
	BROFILER_CATEGORY("ModuleEditor_Preupdate", Profiler::Color::AntiqueWhite)

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::GetIO().DeltaTime = App->time->dt;

	//Editor setup down here
	ImGuiWindowFlags noBackWindowFlags = 0;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse;
	noBackWindowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus;

	//Play-Pause Window
	ImGui::SetNextWindowSize({ SCREEN_WIDTH / 8.0f,50.0f });
	ImGui::SetNextWindowPos({ (SCREEN_WIDTH - SCREEN_WIDTH / 8) / 2.0f,26.0f });
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, { 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, { 0,0,0,0 });
	ImGui::Begin("## Time Buttons", nullptr, noBackWindowFlags);
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	App->time->OnTimeGUI();
	ImGui::End();

	//Guizmos Window
	ImGui::SetNextWindowSize({ SCREEN_WIDTH / 4.0f,50.0f });
	ImGui::SetNextWindowPos({ SCREEN_WIDTH / 4.0f,26.0f });
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Border, { 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, { 0,0,0,0 });
	ImGui::Begin("## Transform Buttons", nullptr, noBackWindowFlags);
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	App->scene->TransformGUI();
	ImGui::End();

	if(showdemowindow)
		ImGui::ShowDemoWindow(&showdemowindow);
	
	if (textEditor)
		showTextEditor();

	if (showConfig)
	{	
		ImGui::SetNextWindowSize(ImVec2(400,400), ImGuiCond_FirstUseEver);

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
			ImGui::SliderFloat("\tGame Scale Speed", &App->time->timeScale, 0.0f, 4.0f);

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
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH/4, SCREEN_HEIGHT/4*3), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4 ), ImGuiCond_FirstUseEver);
		ImGui::Begin("LogWindow", &logEnabled);
		Debug.Draw();
		ImGui::End();
	}

	if (showAbout)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

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
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

		ImGui::Begin("Properties",&propWindow);

		ImGui::End();
	}

	if (hierarchy) 
	{
		ImGui::SetNextWindowPos(ImVec2(0, 23), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 * 3 - 23), ImGuiCond_FirstUseEver);
		if(ImGui::Begin("Hierarchy", &hierarchy, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar))
		{
			App->scene->guiHierarchy();
		}
		ImGui::End();
	}

	if (inspector)
	{
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 4 * 3, 23), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 4, SCREEN_HEIGHT - 23), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Inspector", &inspector, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar))
		{
			App->scene->guiInspector();
		}
		ImGui::End();
	}

	bool clickedRenameMenu = false; //Due to an error with ImGui's IDs

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{			
			if (ImGui::MenuItem("Rename Loaded Scene"))
			{
				clickedRenameMenu = true;			
			}
			
			if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
			{
				if (!IN_GAME)
					App->scene->Serialize();
				else
					Debug.LogWarning("Exit PlayMode before saving new Scenes!");
			}				

			if (ImGui::MenuItem("Quit"))
				return update_status::UPDATE_STOP;

			ImGui::EndMenu();
		}

		

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Demo Window", "", &showdemowindow);
				
			ImGui::MenuItem("Inspector", "", &inspector);

			ImGui::MenuItem("Hierarchy", "", &hierarchy);

			ImGui::MenuItem("FileSystem", "", &fileSystem);

			ImGui::MenuItem("LOG", "", &logEnabled);

			ImGui::MenuItem("Config", "", &showConfig);

			ImGui::MenuItem("Properties", "", &propWindow);

			ImGui::MenuItem("About", "", &showAbout);

			ImGui::MenuItem("Text Editor", "", &textEditor);

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

	if(clickedRenameMenu)
		ImGui::OpenPopup("#### Renaming Scene");

	App->scene->OnRenameGUI();

	//Menu ShortCuts:
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		if (!IN_GAME)
			App->scene->Serialize();
		else
			Debug.LogWarning("Exit PlayMode before saving new Scenes!");
	}
	
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	BROFILER_CATEGORY("EditorUpdate", Profiler::Color::Azure)

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		showdemowindow = false;
		showMGLwindow = false;
		somethingiscolliding = false;
		logEnabled = false;
		showConfig = false;
		showAbout = false;
		propWindow = false;
		hierarchy = false;
		inspector = false;
		fileSystem = false;
		textEditor = false;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	if (fileSystem)
	{
		ImGui::SetNextWindowPos(ImVec2(0, SCREEN_HEIGHT / 4 * 3), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4), ImGuiCond_FirstUseEver);

		ImGui::Begin("FileSystem", &fileSystem, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);

		App->fs->fileSystemGUI();

		ImGui::End();
	}
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	return true;
}

void LogWindow::Draw()
{
	if (ImGui::Button("Clear", ImVec2(80,25)))
	{
		Clear();
	}

	ImGui::BeginTabBar("");
	ImGui::Separator();
	ImGui::BeginChild("scrolling", {0,0}, false, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	logsTitle = std::string(std::string("Logs (") + std::to_string(numLogs) + std::string(")")).c_str();
	if (ImGui::AddTab(logsTitle.data()))
	{
		ImGui::TextUnformatted(NormalBuf.begin());
	}

	warningTitle = std::string(std::string("Warnings (") + std::to_string(numwarnings) + std::string(")")).c_str();
	if (ImGui::AddTab(warningTitle.data()))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 0, 255));
		ImGui::TextUnformatted(WarningBuf.begin());
		ImGui::PopStyleColor();
	}

	errorTitle = std::string(std::string("Errors (") + std::to_string(numerrors) + std::string(")")).c_str();
	if (ImGui::AddTab(errorTitle.data()))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
		ImGui::TextUnformatted(ErrorBuf.begin());
		ImGui::PopStyleColor();
	}
	ImGui::EndTabBar();
	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;
}

void ModuleEditor::Draw() const
{
	BROFILER_CATEGORY("ModuleEditor_Postupdate", Profiler::Color::AntiqueWhite)

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();

}

void ModuleEditor::customStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Separator] = style->Colors[ImGuiCol_Border];
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void ModuleEditor::showTextEditor()
{
	imp();
}
