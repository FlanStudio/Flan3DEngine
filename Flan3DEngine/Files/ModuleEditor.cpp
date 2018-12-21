#include "Application.h"
#include "ModuleEditor.h"
#include "Globals.h"
#include "TextEditor.h"

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
	//// Create application window
	////WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("ImGui Example"), NULL };
	////RegisterClassEx(&wc);
	////HWND hwnd = CreateWindow(_T("ImGui Example"), _T("ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	//// Initialize Direct3D
	////if (CreateDeviceD3D(hwnd) < 0)
	////{
	////	CleanupDeviceD3D();
	////	UnregisterClass(_T("ImGui Example"), wc.hInstance);
	////	return 1;
	////}

	//// Show the window
	////ShowWindow(hwnd, SW_SHOWDEFAULT);
	////UpdateWindow(hwnd);

	//ImGui::CreateContext();

	//// Setup ImGui binding
	////ImGui_ImplWin32_Init(hwnd);
	////ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	//// Load Fonts
	//// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
	//ImVec4 clear_col = ImColor(114, 144, 154);

	/////////////////////////////////////////////////////////////////////////
	//// TEXT EDITOR SAMPLE
	//TextEditor editor;
	//auto lang = TextEditor::LanguageDefinition::CPlusPlus();

	//// set your own known preprocessor symbols...
	//static const char* ppnames[] = { "NULL", "PM_REMOVE",
	//	"ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE", "WINAPI","D3D11_SDK_VERSION", "assert" };
	//// ... and their corresponding values
	//static const char* ppvalues[] = {
	//	"#define NULL ((void*)0)",
	//	"#define PM_REMOVE (0x0001)",
	//	"Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ",
	//	"enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0",
	//	"enum D3D_FEATURE_LEVEL",
	//	"enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
	//	"#define WINAPI __stdcall",
	//	"#define D3D11_SDK_VERSION (7)",
	//	" #define assert(expression) (void)(                                                  \n"
	//	"    (!!(expression)) ||                                                              \n"
	//	"    (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
	//	" )"
	//};

	//for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
	//{
	//	TextEditor::Identifier id;
	//	id.mDeclaration = ppvalues[i];
	//	lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
	//}

	//// set your own identifiers
	//static const char* identifiers[] = {
	//	"HWND", "HRESULT", "LPRESULT","D3D11_RENDER_TARGET_VIEW_DESC", "DXGI_SWAP_CHAIN_DESC","MSG","LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//	"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//	"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//	"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "TextEditor" };
	//static const char* idecls[] =
	//{
	//	"typedef HWND_* HWND", "typedef long HRESULT", "typedef long* LPRESULT", "struct D3D11_RENDER_TARGET_VIEW_DESC", "struct DXGI_SWAP_CHAIN_DESC",
	//	"typedef tagMSG MSG\n * Message structure","typedef LONG_PTR LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
	//	"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
	//	"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
	//	"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "class TextEditor" };
	//for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
	//{
	//	TextEditor::Identifier id;
	//	id.mDeclaration = std::string(idecls[i]);
	//	lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	//}
	//editor.SetLanguageDefinition(lang);
	////editor.SetPalette(TextEditor::GetLightPalette());

	//// error markers
	//TextEditor::ErrorMarkers markers;
	//markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
	//markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
	//editor.SetErrorMarkers(markers);

	//// "breakpoint" markers
	////TextEditor::Breakpoints bpts;
	////bpts.insert(24);
	////bpts.insert(47);
	////editor.SetBreakpoints(bpts);

	//static const char* fileToEdit = "Assets/Scripts/TestScript.cs";
	////	static const char* fileToEdit = "test.cpp";

	//{
	//	std::ifstream t(fileToEdit);
	//	if (t.good())
	//	{
	//		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	//		editor.SetText(str);
	//	}
	//}

	//// Main loop
	//MSG msg;
	//ZeroMemory(&msg, sizeof(msg));
	//while (msg.message != WM_QUIT)
	//{
	//	if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//		continue;
	//	}
	//	//ImGui_ImplDX11_NewFrame();
	//	//ImGui_ImplWin32_NewFrame();
	//	ImGui::NewFrame();

	//	ImGui::ShowTestWindow();

	//	auto cpos = editor.GetCursorPosition();
	//	ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	//	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	//	if (ImGui::BeginMenuBar())
	//	{
	//		if (ImGui::BeginMenu("File"))
	//		{
	//			if (ImGui::MenuItem("Save"))
	//			{
	//				auto textToSave = editor.GetText();
	//				/// save text....
	//			}
	//			if (ImGui::MenuItem("Quit", "Alt-F4"))
	//				break;
	//			ImGui::EndMenu();
	//		}
	//		if (ImGui::BeginMenu("Edit"))
	//		{
	//			bool ro = editor.IsReadOnly();
	//			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
	//				editor.SetReadOnly(ro);
	//			ImGui::Separator();

	//			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
	//				editor.Undo();
	//			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
	//				editor.Redo();

	//			ImGui::Separator();

	//			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
	//				editor.Copy();
	//			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
	//				editor.Cut();
	//			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
	//				editor.Delete();
	//			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
	//				editor.Paste();

	//			ImGui::Separator();

	//			if (ImGui::MenuItem("Select all", nullptr, nullptr))
	//				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

	//			ImGui::EndMenu();
	//		}

	//		if (ImGui::BeginMenu("View"))
	//		{
	//			if (ImGui::MenuItem("Dark palette"))
	//				editor.SetPalette(TextEditor::GetDarkPalette());
	//			if (ImGui::MenuItem("Light palette"))
	//				editor.SetPalette(TextEditor::GetLightPalette());
	//			if (ImGui::MenuItem("Retro blue palette"))
	//				editor.SetPalette(TextEditor::GetRetroBluePalette());
	//			ImGui::EndMenu();
	//		}
	//		ImGui::EndMenuBar();
	//	}

	//	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
	//		editor.IsOverwrite() ? "Ovr" : "Ins",
	//		editor.CanUndo() ? "*" : " ",
	//		editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

	//	editor.Render("TextEditor");
	//	ImGui::End();

	//	// Rendering
	//	//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_col);
	//	//ImGui::Render();
	//	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//	//g_pSwapChain->Present(1, 0); // Present with vsync
	//	//g_pSwapChain->Present(0, 0); // Present without vsync
	//}

	////ImGui_ImplDX11_Shutdown();
	////ImGui_ImplWin32_Shutdown();
	////CleanupDeviceD3D();
	////UnregisterClass(_T("ImGui Example"), wc.hInstance);

}
