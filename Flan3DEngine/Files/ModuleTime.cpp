#include "ModuleTime.h"
#include "Application.h"

bool ModuleTime::Init()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{
	//dt calculation
	dt = dtTimer.Read() / 1000.0f;
	dtTimer.Start();

	//Frame counter
	frameCount++;

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

	//play Dt calculations---------------------(provisional)
	//if (gameModeEnabled)
	//{
	//	playDt = gameDtTimer.Read() / 1000.0f * timeScale;
	//	gameDtTimer.Start();

	//	gameTime += playDt;
	//}
	//-----------------PROVISIONAL--since we have gamemode

	return UPDATE_CONTINUE;
}

update_status ModuleTime::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp()
{
	return true;
}
