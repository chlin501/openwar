// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SurfaceAdapter_Web.h"

#include "Animation.h"
#include "Surface.h"
#include "Gesture.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/Viewport.h"
#include <iostream>


SurfaceAdapter* SurfaceAdapter::_surfaceAdapter = nullptr;


void SurfaceAdapter_KeyDown(char key)
{
	if (SurfaceAdapter::_surfaceAdapter)
		SurfaceAdapter::_surfaceAdapter->KeyDown(key);
}


void SurfaceAdapter_KeyUp(char key)
{
	if (SurfaceAdapter::_surfaceAdapter)
		SurfaceAdapter::_surfaceAdapter->KeyUp(key);
}


void SurfaceAdapter_MouseUpdate(float x, float y, int buttons)
{
	if (SurfaceAdapter::_surfaceAdapter)
		SurfaceAdapter::_surfaceAdapter->MouseUpdate(x, y, buttons);
}


void SurfaceAdapter_MouseWheel(float x, float y, float dx, float dy)
{
	if (SurfaceAdapter::_surfaceAdapter)
		SurfaceAdapter::_surfaceAdapter->MouseWheel(x, y, dx, dy);
}


/***/


SurfaceAdapter::SurfaceAdapter(const char* title)
{
	_surfaceAdapter = this;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	_window = SDL_CreateWindow(title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1024, 768,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	_glcontext = SDL_GL_CreateContext(_window);

	//Uint32 windowID = SDL_GetWindowID(_window);
}


SurfaceAdapter::~SurfaceAdapter()
{
	if (_surfaceAdapter == this)
		_surfaceAdapter = nullptr;
}


void SurfaceAdapter::SetSurface(Surface* surface)
{
	_surface = surface;
	_surface->SetNativeSize((glm::vec2)GetWindowSize());
}


void SurfaceAdapter::Tick()
{
	if (_surfaceAdapter)
	{
		_surfaceAdapter->Update();
		_surfaceAdapter->Render();
	}
}


void SurfaceAdapter::Update()
{
	_surface->SetNativeSize((glm::vec2)GetWindowSize());

	std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
	double secondsSinceTimeStart = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - _timestart).count();
	_timestamp = timestamp;

	AnimationHost::Tick();

	if (_mouse)
	{
		double oldTimestamp = _mouse->GetTimestamp();
		_mouse->Update(secondsSinceTimeStart);

		if (_mouse->GetTimestamp() != oldTimestamp)
			_mouse->TouchMoved();
	}
}


void SurfaceAdapter::Render()
{
	if (_surface)
	{
		_surface->RenderViews();
		SDL_GL_SwapWindow(_window);
	}
}


void SurfaceAdapter::KeyDown(char key)
{
	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyDown(key);
}


void SurfaceAdapter::KeyUp(char key)
{
	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyUp(key);
}


void SurfaceAdapter::MouseUpdate(float x, float y, int buttons)
{
	if (!_surface)
		return;
	if (!_mouse && buttons == 0)
		return;

	double timestamp = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _timestart).count();

	const int clickCount = 1;
	glm::vec2 position{x, y};
	MouseButtons mouseButtons;
	mouseButtons.left = (buttons & 1) != 0;
	mouseButtons.right = (buttons & 2) != 0;
	mouseButtons.other = (buttons & 4) != 0;

	if (!_mouse)
	{
		_mouse = new Touch(1, position, timestamp, mouseButtons);
		_surface->NotifyViewsOfTouchBegin(_mouse);
		_mouse->TouchBegin();
		_mouse->TouchBegan();
	}
	else
	{
		_mouse->Update(position, timestamp, mouseButtons);

		if (_mouse->GetCurrentButtons().Any())
			_mouse->TouchMoved();
	}

	if (!_mouse->GetCurrentButtons().Any())
	{
		_mouse->TouchEnded();
		delete _mouse;
		_mouse = nullptr;
	}


};


void SurfaceAdapter::MouseWheel(float x, float y, float dx, float dy)
{
	glm::vec2 position{x, y};
	glm::vec2 delta = 0.01f * glm::vec2{dx, dy};

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->ScrollWheel(position, delta);
}


glm::ivec2 SurfaceAdapter::GetWindowSize() const
{
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	return glm::ivec2(w, h);
}
