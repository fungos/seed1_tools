#include "app.h"

ISceneNode *pScene;
ISceneNode *pSceneStatic;
App *pApp;

int main(int argc, char **argv)
{
	App app;
	pApp = &app;

	Seed::SetGameApp(&app, argc, argv);
	if (Seed::Initialize())
	{
		if (argv[1] && STRCASECMP(argv[1], "--fullscreen") == 0)
			pScreen->ToggleFullscreen();

		while (!pSystem->IsShuttingDown())
		{
			if (!app.HasError())
			{
				Seed::Update();
			}
			pTimer->Sleep(1);
		}
	}
	
	Seed::Shutdown();
	
	return EXIT_SUCCESS;
}
