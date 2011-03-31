#include "app.h"

App::App()
{
	cConfig.SetVideoMode(Seed::Video_iPhone);
	cConfig.SetPlatformSimulation(Seed::SimulateIOS4G);
	cConfig.SetFrameRate(Seed::FrameRateLockAt60);
	//cConfig.bDebugSprite = TRUE;
	cConfig.SetApplicationTitle("My awesome game");
	cConfig.SetApplicationDescription("My awesome game description");
}

App::~App()
{
}

BOOL App::Initialize()
{
	IGameApp::Initialize();
	
	/* ------- Rendering Initialization ------- */
	cScene.SetPriority(0);
	cSceneStatic.SetPriority(20000);

	cRenderer.Add(&cScene);
	cRenderer.Add(&cSceneStatic);

	cViewport.SetRenderer(&cRenderer);
	pViewManager->Add(&cViewport);
	pRendererManager->Add(&cRenderer);

	pSceneManager->Add(&cScene);
	pSceneManager->Add(&cSceneStatic);

	pScene = &cScene;
	pSceneStatic = &cSceneStatic;
	/* ------- Rendering Initialization ------- */

	sptLogo.Load(SPT_SEED_LOGO, &cResourceManager);
	sptLogo.SetPosition(0.0f, 0.0f);
	sptLogo.SetVisible(TRUE);
	cScene.Add(&sptLogo);
	pScreen->FadeIn();

	return TRUE;
}

BOOL App::Update(f32 dt)
{
	return TRUE;
}

BOOL App::Shutdown()
{
	sptLogo.Unload();
	
	return IGameApp::Shutdown();
}

BOOL App::Reset()
{
	return TRUE;
}

BOOL App::HasError() const
{
	return FALSE;
}

