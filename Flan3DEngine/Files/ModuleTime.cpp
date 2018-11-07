#include "ModuleTime.h"
#include "Application.h"
#include "Brofiler\Brofiler.h"

bool ModuleTime::Init()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{
	BROFILER_CATEGORY("TimePreUpdate", Profiler::Color::Azure)

	//dt calculation
	//dt = (float)dtTimer.Read() / 1000.0f;

	////play Dt calculations---------------------(provisional)
	//if (gameModeEnabled)
	//{
	//	playDt = dt * timeScale;
	//	gameDtTimer.Start();

	//	gameTime += playDt;
	//}
	////-----------------PROVISIONAL--since we have gamemode

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

	if (timer < 5.0f)//COMPROVATION (DELETE)
	{
		timer += dt;
	}
	else
		int y = 9;

	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp()
{
	return true;
}
