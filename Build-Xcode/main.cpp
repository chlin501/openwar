// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <iostream>

#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif

#ifdef OPENWAR_USE_SDL
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#ifdef OPENWAR_USE_SDL_MIXER
#include <SDL2_mixer/SDL_mixer.h>
#endif

#include "OpenWarSurface.h"
#include "Surface/SurfaceAdapter_SDL.h"
#include "BattleMap/BattleMap.h"
#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleSimulator_v1_0_0.h"
#include "BattleScript/BattleScript.h"
#include "BattleScript/PracticeScript.h"
#include "Graphics/Viewport.h"


static BattleScenario* CreateBattleScenario()
{
	Resource res("Maps/Practice.png");
	if (!res.load())
		return nullptr;

	auto smoothMap = std::unique_ptr<Image>(new Image());
	smoothMap->LoadFromResource(res);

	SmoothGroundMap* groundMap = new SmoothGroundMap(bounds2f(0, 0, 1024, 1024), std::move(smoothMap));
	BattleMap* battleMap = new BasicBattleMap(groundMap->GetHeightMap(), groundMap);

	BattleSimulator* battleSimulator = new BattleSimulator_v1_0_0(battleMap);

	BattleScenario* battleScenario = new BattleScenario(battleSimulator, 0);
	battleScenario->SetPractice(true);
	battleScenario->SetBattleScript(new PracticeScript(battleScenario));
    BattleCommander* player = battleScenario->AddCommander("1", 1, BattleCommanderType::Player);
    battleScenario->AddCommander("2", 2, BattleCommanderType::Script);

	glm::vec2 center(512, 512);

    battleSimulator->AddUnit(player, "SAM-BOW", 80, center + glm::vec2(-50, 0), 0);
    battleSimulator->AddUnit(player, "SAM-ARQ", 80, center + glm::vec2(  0, 0), 0);
    battleSimulator->AddUnit(player, "SAM-BOW", 80, center + glm::vec2( 50, 0), 0);
    battleSimulator->AddUnit(player, "SAM-YARI", 80, center + glm::vec2(-25, -30), 0);
    battleSimulator->AddUnit(player, "SAM-YARI", 80, center + glm::vec2( 25, -30), 0);
    battleSimulator->AddUnit(player, "SAM-KATA", 80, center + glm::vec2(-50, -60), 0);
    battleSimulator->AddUnit(player, "GEN-KATA", 40, center + glm::vec2(  0, -60), 0);
    battleSimulator->AddUnit(player, "SAM-KATA", 80, center + glm::vec2( 50, -60), 0);
    battleSimulator->AddUnit(player, "CAV-YARI", 40, center + glm::vec2(-70, -100), 0);
    battleSimulator->AddUnit(player, "SAM-NAGI", 80, center + glm::vec2(  0, -90), 0);
    battleSimulator->AddUnit(player, "CAV-BOW",  40, center + glm::vec2( 70, -100), 0);

	return battleScenario;
}


int main(int argc, char *argv[])
{
    Resource::init(argv[0]);

#ifdef OPENWAR_USE_SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();
#endif
#ifdef OPENWAR_USE_SDL_MIXER
	Mix_Init(0);
#endif

	SurfaceAdapter* surfaceAdapter = new SurfaceAdapter("openwar");

#if OPENWAR_USE_GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glewInit() -> " << glewGetErrorString(err) << std::endl;
		return -1;
	}
#endif
    
    GraphicsContext* gc = new GraphicsContext(1, 1);
	OpenWarSurface* surface = new OpenWarSurface(gc);

	surfaceAdapter->SetSurface(surface);

	BattleScenario* battleScenario = CreateBattleScenario();
	std::vector<BattleCommander*> battleCommanders(1, battleScenario->GetCommanders().front());
	surface->ResetBattleViews(battleScenario, battleCommanders);

	while (!SurfaceAdapter::IsDone())
		SurfaceAdapter::ProcessEvents();

	SDL_Quit();

	return 0;
}
