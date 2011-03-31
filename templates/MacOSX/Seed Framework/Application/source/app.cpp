#include "app.h"

App::App()
{
	pSystem->AddListener(this);
	pInput->AddKeyboardListener(this);
	
	cConfig.SetRendererDeviceType(Seed::RendererDeviceOpenGL14);
	cConfig.SetVideoMode(Seed::Video_1024x768);
	cConfig.SetFrameRate(Seed::FrameRateLockAt30);
	//cConfig.bDebugSprite = TRUE;
	cConfig.SetApplicationTitle("My Application");
	cConfig.SetApplicationDescription("My Application Description");
}

App::~App()
{
	pInput->RemoveKeyboardListener(this);
	pSystem->RemoveListener(this);
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
	//pScreen->FadeIn();

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

BOOL App::Update(f32 dt)
{
	return TRUE;
}

void App::OnSystemShutdown(const EventSystem *ev)
{
	pSystem->Shutdown();
}

void App::OnInputKeyboardRelease(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();

	switch (k.GetValue())
	{
		case Seed::KeyEscape:
		{
			pSystem->Shutdown();
		}
		break;

		default:
		break;
	}
}

