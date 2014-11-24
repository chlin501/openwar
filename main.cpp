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
#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleScript.h"
#include "BattleModel/PracticeScript.h"
#include "Viewport.h"

static BattleScenario* CreateBattleScenario()
{
	BattleScenario* scenario = new BattleScenario();
	scenario->SetScript(new PracticeScript(scenario));
	scenario->AddCommander("1", 1, BattleCommanderType::Player);
	scenario->AddCommander("2", 2, BattleCommanderType::Script);
	scenario->StartScript();
	return scenario;
}



int main(int argc, char *argv[])
{
    resource::init(argv[0]);

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
    
    GraphicsContext* gc = new GraphicsContext(1);
	OpenWarSurface* surface = new OpenWarSurface(gc);
    surface->SetSize(glm::vec2(640, 480));
    RenderLoopObserver::NotifyRenderLoop(0);

	window->SetSurface(surface);

	BattleScenario* scenario = CreateBattleScenario();
	std::vector<BattleCommander*> commanders(1, scenario->GetCommanders().front());
	surface->ResetBattleViews(scenario, commanders);

	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();

	return 0;
}
