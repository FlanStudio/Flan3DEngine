#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui/imgui.h"
#include "Parson/parson.h"
#include "MathGeoLib_1.5/Math/MathConstants.h"

#include "Brofiler\Brofiler.h"

#pragma comment( lib, "Brofiler/ProfilerCore32.lib")
#pragma comment (lib, "Glew/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("ModuleRenderer", start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	Debug.Log("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		Debug.LogError("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if (ret)
	{
		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if (GLEW_OK != error)
		{
			Debug.LogError("Init glew failed. Error: %s\n", glewGetErrorString(error));
			ret = false;
		}
		else
		{
			Debug.Log("Using Glew %s", glewGetString(GLEW_VERSION));
		}
	}
	

	if(ret == true)
	{
		//Use Vsync
		if (VSYNC)
		{
			if (SDL_GL_SetSwapInterval(1) < 0)
				Debug.LogError("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
		}

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			Debug.Log("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			Debug.Log("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			Debug.Log("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		depthTest = true;
		glEnable(GL_CULL_FACE);
		cullFace = true;
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		lighting = true;
		glEnable(GL_COLOR_MATERIAL);
		colorMaterial = true;


	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	/*cube.Init();
	cube.Rotate(40, -1, -1, -1);
	cube.setColor({ .5, 0, 0, 1 });*/
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleRenderer3D_Preupdate", Profiler::Color::Azure)

	Color color = App->camera->background;
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	//// light 0 on cam pos
	//lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	//for(uint i = 0; i < MAX_LIGHTS; ++i)
	//	lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ModuleRenderer3D_Postupdate", Profiler::Color::Azure)


	//cube.Render();
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	Debug.Log("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = Perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(ProjectionMatrix.ptr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::guiGPU()const
{
	ImGui::Text("Brand: "); ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%s", glGetString(GL_VENDOR));

	ImGui::Text("GPU: "); ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%s", glGetString(GL_RENDERER));

	ImGui::Text("OpenGL version supported: "); ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%s", glGetString(GL_VERSION));

	ImGui::Text("GLSL: "); ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	ImGui::NewLine();
}

void ModuleRenderer3D::guiRenderer()
{
	if(ImGui::Checkbox("DEPTH_TEST", &depthTest))
	{
		if (depthTest)
		{
			glEnable(GL_DEPTH_TEST);
			Debug.Log("Renderer: DEPTH_TEST enabled");
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			Debug.Log("Renderer: DEPTH_TEST disabled");
		}	
	} ImGui::SameLine();
	if (ImGui::Checkbox("CULL_FACE", &cullFace))
	{
		if (cullFace)
		{
			glEnable(GL_CULL_FACE);
			Debug.Log("Renderer: CULL_FACE enabled");
		}
		else
		{
			glDisable(GL_CULL_FACE);
			Debug.Log("Renderer: CULL_FACE disabled");
		}
	}
	if (ImGui::Checkbox("LIGHTNING ", &lighting))
	{
		if (lighting)
		{
			glEnable(GL_LIGHTING);
			Debug.Log("Renderer: LIGHTING enabled");
		}
		else
		{
			glDisable(GL_LIGHTING);
			Debug.Log("Renderer: LIGHTING disabled");
		}
	} ImGui::SameLine();
	if (ImGui::Checkbox("COLOR_MATERIAL", &colorMaterial))
	{
		if (colorMaterial)
		{
			glEnable(GL_COLOR_MATERIAL);
			Debug.Log("Renderer: COLOR_MATERIAL enabled");
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
			Debug.Log("Renderer: COLOR_MATERIAL disabled");
		}
	}
	if (ImGui::Checkbox("TEXTURE2D ", &texture2D))
	{
		if (texture2D)
		{
			glEnable(GL_TEXTURE_2D);
			Debug.Log("Renderer: TEXTURE_2D enabled");
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			Debug.Log("Renderer: TEXTURE_2D disabled");
		}
	} ImGui::SameLine();
	if (ImGui::Checkbox("WIREFRAME_MODE", &wireframe))
	{
		setWireframe(wireframe);
	}
}

bool ModuleRenderer3D::Save(JSON_Object* obj) const
{
	json_object_set_boolean(obj, "depthTest", depthTest);
	json_object_set_boolean(obj, "cullFace", cullFace);
	json_object_set_boolean(obj, "lighting", lighting);
	json_object_set_boolean(obj, "colorMaterial", colorMaterial);
	json_object_set_boolean(obj, "texture2D", texture2D);
	json_object_set_boolean(obj, "wireframeMode", wireframe);
	return true;
}

bool ModuleRenderer3D::Load(const JSON_Object* obj)
{
	depthTest = json_object_get_boolean(obj, "depthTest");
	cullFace = json_object_get_boolean(obj, "cullFace");
	lighting = json_object_get_boolean(obj, "lighting");
	colorMaterial = json_object_get_boolean(obj, "colorMaterial");
	texture2D = json_object_get_boolean(obj, "texture2D");
	wireframe = json_object_get_boolean(obj, "wireframeMode");

	depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	colorMaterial ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
	texture2D ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	setWireframe(wireframe);

	return true;
}

void ModuleRenderer3D::setWireframe(bool boolean)
{
	wireframe = boolean;
	glPolygonMode(GL_FRONT_AND_BACK, boolean ? GL_LINE : GL_FILL);
	//Debug.Log("Renderer: WIREFRAME_MODE %s", boolean ? "enabled" : "disabled");
}

float4x4 ModuleRenderer3D::Perspective(float fovy, float aspect, float znear, float zfar)
{
	math::float4x4 Perspective;

	float coty = 1.0f / tan(fovy * (float)pi / 360.0f);

	Perspective[0][0] = coty / aspect;
	Perspective[0][1] = 0.0f;
	Perspective[0][2] = 0.0f;
	Perspective[0][3] = 0.0f;
	Perspective[1][0] = 0.0f;
	Perspective[1][1] = coty;
	Perspective[1][2] = 0.0f;
	Perspective[1][3] = 0.0f;
	Perspective[2][0] = 0.0f;
	Perspective[2][1] = 0.0f;
	Perspective[2][2] = (znear + zfar) / (znear - zfar);
	Perspective[2][3] = -1.0f;
	Perspective[3][0] = 0.0f;
	Perspective[3][1] = 0.0f;
	Perspective[3][2] = 2.0f * znear * zfar / (znear - zfar);
	Perspective[3][3] = 0.0f;

	return Perspective;
}