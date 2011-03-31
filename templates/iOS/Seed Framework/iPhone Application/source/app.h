#ifndef __APP_H__
#define __APP_H__

#include <Seed.h>
#include "assets.h"

extern ISceneNode *pScene;
extern ISceneNode *pSceneStatic;

using namespace Seed;

class App : public IGameApp
{
	public:
		App();
		~App();

		virtual BOOL Initialize();
		virtual BOOL Update(f32 dt);
		virtual BOOL Reset();
		virtual BOOL Shutdown();
		virtual BOOL HasError() const;
		
	private:
		SEED_DISABLE_COPY(App);

	protected:
		SceneNode<1024> cScene;
		SceneNode<32> cSceneStatic;
		Viewport	cViewport;
		Renderer	cRenderer;
		
		Sprite		sptLogo;
};

#endif // __APP_H__
