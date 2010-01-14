#include "mygame.h"

int main(int argc, char **argv)
{
	MyGame app;

	Seed::SetGameApp(&app, argc, argv);
	Seed::Initialize();
	while (!pSystem->IsShuttingDown())
	{
		Seed::Update();
	}
	Seed::Shutdown();

	return EXIT_SUCCESS;
}
