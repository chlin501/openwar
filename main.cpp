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
#include "Surface/Window.h"
#include "BattleMap/BattleMap.h"
#include "BattleModel/BattleSimulator.h"
#include "BattleModel/BattleScript.h"
#include "BattleModel/PracticeScript.h"
#include "Viewport.h"


static BattleSimulator* CreateBattleSimulator()
{
	Resource res("Maps/Practice.png");
	if (!res.load())
		return nullptr;

	auto smoothMap = std::unique_ptr<Image>(new Image());
	smoothMap->LoadFromResource(res);

	SmoothGroundMap* groundMap = new SmoothGroundMap(bounds2f(0, 0, 1024, 1024), std::move(smoothMap));
	BattleMap* battleMap = new BasicBattleMap(groundMap->GetHeightMap(), groundMap);

	BattleSimulator* battleSimulator = new BattleSimulator(battleMap);
	battleSimulator->SetPractice(true);
	battleSimulator->SetScript(new PracticeScript(battleSimulator));
	battleSimulator->AddCommander("1", 1, BattleCommanderType::Player);
	battleSimulator->AddCommander("2", 2, BattleCommanderType::Script);

	glm::vec2 center(512, 512);

	battleSimulator->NewUnit(1, "SAM-BOW", 80, center + glm::vec2(-50, 0), 0);
	battleSimulator->NewUnit(1, "SAM-ARQ", 80, center + glm::vec2(  0, 0), 0);
	battleSimulator->NewUnit(1, "SAM-BOW", 80, center + glm::vec2( 50, 0), 0);

	battleSimulator->NewUnit(1, "SAM-YARI", 80, center + glm::vec2(-25, -30), 0);
	battleSimulator->NewUnit(1, "SAM-YARI", 80, center + glm::vec2( 25, -30), 0);

	battleSimulator->NewUnit(1, "SAM-KATA", 80, center + glm::vec2(-50, -60), 0);
	battleSimulator->NewUnit(1, "GEN-KATA", 40, center + glm::vec2(  0, -60), 0);
	battleSimulator->NewUnit(1, "SAM-KATA", 80, center + glm::vec2( 50, -60), 0);

	battleSimulator->NewUnit(1, "CAV-YARI", 40, center + glm::vec2(-70, -100), 0);
	battleSimulator->NewUnit(1, "SAM-NAGI", 80, center + glm::vec2(  0, -90), 0);
	battleSimulator->NewUnit(1, "CAV-BOW",  40, center + glm::vec2( 70, -100), 0);

	return battleSimulator;
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

	Window* window = new Window();

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

	window->SetSurface(surface);

    RenderLoopObserver::NotifyRenderLoop(0);

	BattleSimulator* battleSimulator = CreateBattleSimulator();
	std::vector<BattleCommander*> battleCommanders(1, battleSimulator->GetCommanders().front());
	surface->ResetBattleViews(battleSimulator, battleCommanders);

	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();

	return 0;
}
