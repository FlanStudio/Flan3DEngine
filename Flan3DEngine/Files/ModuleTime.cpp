#include "ModuleTime.h"
#include "Application.h"
#include "Brofiler\Brofiler.h"

#include "ResourceTexture.h"

bool ModuleTime::Start()
{
	TimeAtlasID = App->textures->getInternalTextureID("timeAtlas.dds");
	return true;
}

update_status ModuleTime::PreUpdate()
{
	BROFILER_CATEGORY("TimePreUpdate", Profiler::Color::Azure);

	dtTimer.Start();

	//play Dt calculations--------------
	playDt = IN_GAME ? dt * timeScale : 0;
	timer += dt;

	if (IN_GAME)
	{
		if (paused && !steped)
		{
			playDt = 0.0f;
		}
		else if (steped)//if steped ignore playDt = 0.0f during 1 frame
		{
			paused = true;
			steped = false;
		}
		gameTime += playDt;
	}

	return UPDATE_CONTINUE;
}

update_status ModuleTime::PostUpdate()
{
	BROFILER_CATEGORY("TimePostUpdate", Profiler::Color::Azure)

	//Frame counter
	frameCount++;

	static float dtSecondCounter = 0.0f;

	//Frames in 1 sec
	if (dtSecondCounter < 1.0f)
	{
		dtSecondCounter += dt;
		lastSecFramesCounter++;
	}
	else
	{
		lastSecFrames = lastSecFramesCounter;
		lastSecFramesCounter = 0;
		dtSecondCounter = 0.0f;
	}

	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp()
{
	return true;
}

void ModuleTime::OnTimeGUI()
{
	ImVec2 drawingPos = ImGui::GetCursorScreenPos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGui::SetCursorScreenPos({ windowPos.x + 33.0f,windowPos.y + 2.0f });

	ImGui::PushID("playButton");

	if (!IN_GAME)
	{
		if (ImGui::ImageButton((GLuint*)TimeAtlasID, { 25,25 }, { 0.345f,0.0f }, { 0.655f,0.47f }))//play
		{
			IN_GAME = true;
			Event event;
			event.type = EventType::PLAY;
			App->SendEvent(event);
		}
	}
	else
	{
		if (ImGui::ImageButton((GLuint*)TimeAtlasID, { 25,25 }, { 0.0f,0.0f }, { 0.31f, -0.47f }))//stop
		{
			IN_GAME = false;
			paused = false;
			Event event;
			event.type = EventType::STOP;
			App->SendEvent(event);
		}
	}
	ImGui::PopID();
	ImGui::PushID("pauseButton");
	ImGui::SameLine();
	if (!paused)
	{
		if (ImGui::ImageButton((GLuint*)TimeAtlasID, { 25,25 }, { 0.345f,0.0f }, { 0.655f,-0.47f }) && IN_GAME)//pause
		{
			Event event;
			event.type = EventType::PAUSE;
			App->SendEvent(event);
			paused = true;
		}
		ImGui::SameLine();
	}
	else if (IN_GAME)
	{
		if (ImGui::ImageButton((GLuint*)TimeAtlasID, { 25,25 }, { 0.345f,0.0f }, { 0.655f,-0.47f }, -1, { 0.0f,0.0f,0.0f,0.0f }, {0.8f,0.3f,0.3f,1.0f}))//resume
		{
			Event event;
			event.type = EventType::RESUME;
			App->SendEvent(event);
			paused = false;
		}
		ImGui::SameLine();
	}
	ImGui::PopID();
	ImGui::PushID("stepButtons");
	if (ImGui::ImageButton((GLuint*)TimeAtlasID, { 25,25 }, { 0.69f,0.0f }, { 1.0f,0.47f }) && paused)//step
	{
		Event event;
		event.type = EventType::STEP;
		App->SendEvent(event);
		steped = true;
		paused = false;
	}
	ImGui::PopID();
	ImGui::SameLine();
}
