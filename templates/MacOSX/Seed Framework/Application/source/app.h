#ifndef __APP_H__
#define __APP_H__

#include "main.h"

class App : public IGameApp, public IEventSystemListener, public IEventInputKeyboardListener
{
	public:
		App();
		~App();

		virtual BOOL Initialize();
		virtual BOOL Update(f32 dt);
		virtual BOOL Reset();
		virtual BOOL Shutdown();
		virtual BOOL HasError() const;

		// IEventSystemListener
		virtual void OnSystemShutdown(const EventSystem *ev);

		// IEventInputKeyboardListener
		virtual void OnInputKeyboardRelease(const EventInputKeyboard *ev);

	protected:
		SceneNode<1024> cScene;
		SceneNode<32> cSceneStatic;
		Viewport cViewport;
		Renderer cRenderer;
		
		Sprite sptLogo;

	private:
		SEED_DISABLE_COPY(App);
};

#endif // __APP_H__

