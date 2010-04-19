#include "mygame.h"
#include "assets.h"


#define VERSION "Seed Particle Viewer v0.1.8"


const char *table[] = {
	"psi/particle1.psi",
	"psi/particle2.psi",
	"psi/particle3.psi",
	"psi/particle4.psi",
	"psi/particle5.psi",
	"psi/particle6.psi",
	"psi/particle7.psi",
	"psi/particle8.psi",
	"psi/particle9.psi"
};

ResourceManager glMyResources("mine");


MyGame::MyGame()
	: pRenderer(NULL)
	, bFollow(FALSE)
{
}

MyGame::~MyGame()
{
}

void MyGame::Setup(int argc, char **argv)
{
	//pScreen->Setup(Screen::SCREEN_800X600X32W_OPENGL);
	pScreen->Setup(Screen::SCREEN_1024X768X32W_OPENGL);
	pSystem->SetFrameRate(ISystem::RATE_60FPS);
	pSystem->SetApplicationTitle(VERSION);
	pSystem->SetApplicationDescription(VERSION);
	pFileSystem->SetWorkDirectory("");
}

BOOL MyGame::Initialize()
{
	this->pRenderer = new Renderer2D();

	Seed::SetRenderer(this->pRenderer);
	pScreen->SetRenderer(this->pRenderer);

	sptLogo.Load(SPT_BG, &glMyResources);
	sptLogo.SetPosition(0.0f, 0.0f);
	sptLogo.SetVisible(TRUE);
	pRenderer->Add(&sptLogo);

	//strPos.Set(DIC_POS);
	strPos.Set(DIC_POS).Set(DIC_X, 0.0f).Set(DIC_Y, 0.0f);
	glStringPool.OverRunReport();
	glStringPool.PrintSnapshot();

	fntFont.Load(FNT_FONT25, &glMyResources);
	lblPos.SetFont(&fntFont);
	lblPos.SetText(strPos);
	lblPos.SetPriority(1);
	pGuiManager->Add(&lblPos);
	pRenderer->Add(&lblPos);

	cEmitter.Load(table[0], &glMyResources);
	cEmitter.SetSprite(SPT_PARTICLE);
	cEmitter.SetPriority(10);
	cEmitter.SetPosition(0.5f, 0.5f);
	cEmitter.Stop();
	pRenderer->Add(&cEmitter);
	pParticleManager->Add(&cEmitter);

	pInput->AddKeyboardListener(this);
	pInput->AddPointerListener(this);

	return TRUE;
}

BOOL MyGame::Update()
{
	return TRUE;
}

BOOL MyGame::Shutdown()
{
	glMyResources.Reset();
	delete pRenderer;

	return TRUE;
}

BOOL MyGame::Reset()
{
	return TRUE;
}

void MyGame::OnInputKeyboardRelease(const EventInputKeyboard *ev)
{
	Key k = ev->GetKey();

	if (k.IsNumber())
	{
		u32 v = k.GetValue() - '1';
		if (v >= 0 && v < 9)
		{
			cEmitter.Unload();
			cEmitter.Load(table[v], &glMyResources);
			cEmitter.SetSprite(SPT_PARTICLE);
			cEmitter.Play();	
		}
	}
	else
	{
		u32 c = k.GetValue();
		if (c == Seed::KeyTab)
		{
			if (bLinear)
				cEmitter.SetFilter(TextureFilterTypeMag, TextureFilterNearest);
			else
				cEmitter.SetFilter(TextureFilterTypeMag, TextureFilterLinear);

			bLinear = !bLinear;
		}

		if (c == Seed::KeyBackspace)
		{
			if (pParticleManager->IsEnabled())
				pParticleManager->Disable();
			else
				pParticleManager->Enable();
		}

		u32 v = c - static_cast<u32>(Seed::KeyF1);
		if (v >= 0 && v < 15)
			cEmitter.SetAnimation(v);
	}
}

void MyGame::OnInputPointerPress(const EventInputPointer *ev)
{
	bFollow = TRUE;
	cEmitter.SetPosition(ev->GetX(), ev->GetY());
	strPos.Set(DIC_POS).Set(DIC_X, ev->GetX()).Set(DIC_Y, ev->GetY());
}

void MyGame::OnInputPointerRelease(const EventInputPointer *ev)
{
	strPos.Set(DIC_POS).Set(DIC_X, ev->GetX()).Set(DIC_Y, ev->GetY());
	bFollow = FALSE;
}

void MyGame::OnInputPointerMove(const EventInputPointer *ev)
{
	if (bFollow)
	{
		strPos.Set(DIC_POS).Set(DIC_X, ev->GetX()).Set(DIC_Y, ev->GetY());
		//lblPos.SetText(strPos);
		cEmitter.SetPosition(ev->GetX(), ev->GetY());
	}
}

