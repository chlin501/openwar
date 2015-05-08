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
#include "BattleModel/BattleSimulator.h"
#include "BattleModel/BattleScript.h"
#include "BattleModel/PracticeScript.h"
#include "Viewport.h"


static BattleSimulator* CreateBattleSimulator()
{
	BattleSimulator* simulator = new BattleSimulator();
	simulator->SetScript(new PracticeScript(simulator));
	simulator->AddCommander("1", 1, BattleCommanderType::Player);
	simulator->AddCommander("2", 2, BattleCommanderType::Script);
	simulator->LoadLegacySmoothMap("Maps/Practice.png", "Maps/Practice.png", 1024);
	simulator->GetScript()->Execute();
	return simulator;
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
