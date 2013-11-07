// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <iostream>

#if OPENWAR_USE_GLEW
#include <GL/glew.h>
#endif

#ifdef OPENWAR_USE_SDL
#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif
#endif

#include "Sources/OpenWarSurface.h"
#include "Library/ViewCore/Window.h"
#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleScript.h"


static BattleScenario* CreateBattleScenario()
{
	resource source("Maps/DefaultMap.lua");
	source.load();
    
	std::string directory = resource("Maps/").path();
	std::string package_path = directory + "/?.lua";

	BattleScenario* scenario = new BattleScenario();
	scenario->AddCommander("1", BattleCommanderType::Screen, "");
	scenario->AddCommander("2", BattleCommanderType::Script, "");

	BattleScript* script = scenario->GetScript();

	script->SetGlobalNumber("openwar_seed", 0);
	script->SetGlobalString("openwar_script_directory", directory.c_str());
	script->AddStandardPath();
	script->AddPackagePath(package_path.c_str());
    
	script->Execute((const char*)source.data(), source.size());
    
	return scenario;
}



int main(int argc, char *argv[])
{
	std::cout << "Hello" << std::endl;
	std::cout << argv[0] << std::endl;

    resource::init(argv[0]);

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	Window* window = new Window();

#if OPENWAR_USE_GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glewInit() -> " << glewGetErrorString(err) << std::endl;
		return -1;
	}
#endif
    
	OpenWarSurface* surface = new OpenWarSurface(glm::vec2(640, 480), 1);
	window->SetSurface(surface);

	surface->Reset(CreateBattleScenario());
    
	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();
    
	return 0;
}
