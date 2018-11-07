#include "ModuleTime.h"
#include "Application.h"

bool ModuleTime::Init()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{
	//dt calculation
	dt = (float)dtTimer.Read() / 1000.0f;
	dtTimer.Start();

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

	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp()
{
	return true;
}
