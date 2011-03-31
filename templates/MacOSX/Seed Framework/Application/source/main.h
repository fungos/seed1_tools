#ifndef __MAIN_H__
#define __MAIN_H__

#include <Seed.h>
using namespace Seed;

#include "assets.h"

class App;

extern ISceneNode *pScene;
extern ISceneNode *pSceneStatic;
extern App *pApp;

SEED_ENABLE_INSTANCING(Sprite);

#define PRIORITY_BG			(0)
#define PRIORITY_ENTITY		(PRIORITY_BG + 100000)
#define PRIORITY_POWERUP	(PRIORITY_ENTITY + 1)
#define PRIORITY_ENDING		(PRIORITY_POWERUP + 1000)
#define PRIORITY_THANKS		(PRIORITY_ENDING + 1000)

enum eTeam
{
	TeamPanda,
	TeamMaya,

	MaxTeams
};

#endif // __MAIN_H__
