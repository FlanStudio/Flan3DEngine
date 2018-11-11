#include "ModuleTime.h"
#include "Application.h"
#include "Brofiler\Brofiler.h"

#include "ResourceTexture.h"

bool ModuleTime::Start()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{
	BROFILER_CATEGORY("TimePreUpdate", Profiler::Color::Azure)

	dtTimer.Start();

	//play Dt calculations---------------------(provisional)
	//if (play)
	//{
	//	gameTime += playDt;
	//}
	//-----------------PROVISIONAL--since we have gamemode

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
	if (!timeAtlas && App->textures->textures.size() > 0)
		timeAtlas = App->textures->textures[3]; //Temporal, this will be in a hidden directory

	if (!timeAtlas)
		return;

	ImVec2 drawingPos = ImGui::GetCursorScreenPos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGui::SetCursorScreenPos({ windowPos.x + 33.0f,windowPos.y + 2.0f });

	if (!IN_GAME)
	{
		if (ImGui::ImageButton((GLuint*)timeAtlas->id, { 18,18 }, { 0.345f,0.0f }, { 0.655f,0.47f }))//play
		{
			IN_GAME = true;
			Event event;
			event.timeEvent.type = EventType::PLAY;
			App->SendEvent(event);
		}
	}
	else
	{
		if (ImGui::ImageButton((GLuint*)timeAtlas->id, { 18,18 }, { 0.0f,0.0f }, { 0.31f, -0.47f }))//stop
		{
			IN_GAME = false;
		}
	}

	ImGui::SameLine();

	if (ImGui::ImageButton((GLuint*)timeAtlas->id, { 18,18 }, { 0.345f,0.0f }, { 0.655f,-0.47f }))//pause
	{

	}
	ImGui::SameLine();

	if (ImGui::ImageButton((GLuint*)timeAtlas->id, { 18,18 }, { 0.69f,0.0f }, { 1.0f,0.47f }))//step
	{

	}
	ImGui::SameLine();
}
