#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL\include\SDL.h"
#include "Brofiler\Brofiler.h"

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "Brofiler/ProfilerCore32.lib")

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char ** argv)
{
	Debug.Log("Starting game ");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			Debug.Log("-------------- Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			Debug.Log("-------------- Application Init ------------------");
			if (App->Init() == false)
			{
				Debug.LogError("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				Debug.Log("-------------- Application Update -----------------");
			}

			break;

		case MAIN_UPDATE:
		{
			BROFILER_FRAME("YourThreadName")

			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				Debug.LogError("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			Debug.Log("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				Debug.LogError("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	Debug.Log("Exiting game");
	return main_return;
}