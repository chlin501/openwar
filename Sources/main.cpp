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
#include "Library/Surface/Window.h"
#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleScript.h"
#include "BattleModel/PracticeScript.h"

static BattleScenario* CreateBattleScenario()
{
	BattleScenario* scenario = new BattleScenario();
	scenario->SetScript(new PracticeScript(scenario));
	scenario->AddCommander("1", 1, BattleCommanderType::Player)->SetActive(true);
	scenario->AddCommander("2", 2, BattleCommanderType::Script);
	scenario->StartScript(true);
	return scenario;
}


/*
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>


extern "C" {
	GL_APICALL void GL_APIENTRY glGenVertexArraysOES(GLsizei n, GLuint* arrays)
	{
		static PFNGLGENVERTEXARRAYSOESPROC _glGenVertexArraysOES = 0;
		if (_glGenVertexArraysOES == 0)
			_glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress( "glGenVertexArraysOES" );
		if (_glGenVertexArraysOES != 0)
			_glGenVertexArraysOES(n, arrays);
	}

	GL_APICALL void GL_APIENTRY glBindVertexArrayOES(GLuint array)
	{
		static PFNGLBINDVERTEXARRAYOESPROC _glBindVertexArrayOES = 0;
		if (_glBindVertexArrayOES == 0)
			_glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress ( "glBindVertexArrayOES" );
		if (_glBindVertexArrayOES != 0)
			_glBindVertexArrayOES(array);
	}

	GL_APICALL void GL_APIENTRY glDeleteVertexArraysOES(GLsizei n, const GLuint* arrays)
	{
		static PFNGLDELETEVERTEXARRAYSOESPROC _glDeleteVertexArraysOES = 0;
		if (_glDeleteVertexArraysOES == 0)
			_glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress ( "glDeleteVertexArraysOES" );
		if (_glDeleteVertexArraysOES != 0)
			_glDeleteVertexArraysOES(n, arrays);
	}

	GL_APICALL GLboolean GL_APIENTRY glIsVertexArrayOES(GLuint array)
	{
		static PFNGLISVERTEXARRAYOESPROC _glIsVertexArrayOES = 0;
		if (_glIsVertexArrayOES == 0)
			_glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress ( "glIsVertexArrayOES" );
		if (_glIsVertexArrayOES != 0)
			return _glIsVertexArrayOES(array);
		return false;
	}
}
*/

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

	BattleScenario* scenario = CreateBattleScenario();
	std::vector<BattleCommander*> commanders(1, scenario->GetCommanders().front());
	surface->ResetBattleViews(scenario, commanders);
    
	while (!Window::IsDone())
		Window::ProcessEvents();

	SDL_Quit();
    
	return 0;
}
