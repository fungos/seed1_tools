#ifndef __MYGAME_H__
#define __MYGAME_H__

#include <Seed.h>

using namespace Seed;

#ifdef _SDL_
#define VIDEO_MODE	mode
#endif

#ifdef _IPHONE_
#define VIDEO_MODE Screen::LANDSCAPE
#endif

#ifdef _WII_
#define VIDEO_MODE
#endif

class MyGame : public IGameApp, public IEventInputKeyboardListener, public IEventInputPointerListener
{
	public:
		MyGame();
		~MyGame();

		virtual void Setup(int argc, char **argv);
		virtual BOOL Initialize();
		virtual BOOL Update();
		virtual BOOL Reset();
		virtual BOOL Shutdown();
	
		// IEventInputKeyboardListener
		virtual void OnInputKeyboardRelease(const EventInputKeyboard *ev);
	
		// IEventInputPointerListener
		virtual void OnInputPointerPress(const EventInputPointer *ev);
		virtual void OnInputPointerRelease(const EventInputPointer *ev);
		virtual void OnInputPointerMove(const EventInputPointer *ev);

	private:
		SEED_DISABLE_COPY(MyGame);

	private:
		Renderer2D 	*pRenderer;
		Sprite		sptLogo;
		Label		lblPos;
		Font		fntFont;
		String		strPos;

		ParticleEmitter cEmitter;
		BOOL		bFollow;
		BOOL		bLinear;
};

#endif // __MYGAME_H__

