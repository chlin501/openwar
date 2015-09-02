// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "SurfaceAdapter_SDL.h"

#include "Animation.h"
#include "Surface.h"
#include "Gesture.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/Viewport.h"


bool SurfaceAdapter::_done = false;
std::map<Uint32, SurfaceAdapter*> SurfaceAdapter::_windows;
SurfaceAdapter* SurfaceAdapter::_touchWindow = nullptr;


SurfaceAdapter::SurfaceAdapter(const char* title) :
_surface(nullptr),
_window(nullptr),
_glcontext(0),
_mouseTouch(nullptr),
_timestart(),
_timestamp()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	_window = SDL_CreateWindow(title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1024, 768,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	_glcontext = SDL_GL_CreateContext(_window);

	Uint32 windowID = SDL_GetWindowID(_window);

	_windows[windowID] = this;
	_touchWindow = this;

	_timestart = std::chrono::system_clock::now();
	_timestamp = _timestart;
}


SurfaceAdapter::~SurfaceAdapter()
{
}


void SurfaceAdapter::SetSurface(Surface* surface)
{
	_surface = surface;
	_surface->SetNativeSize((glm::vec2)GetWindowSize());
}


bool SurfaceAdapter::IsDone()
{
	return _done;
}


void SurfaceAdapter::ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ProcessEvent(event);
	}

	for (auto i : _windows)
	{
		if (i.second)
		{
			i.second->Update();
			i.second->Render();
		}
	}
}


SurfaceAdapter* SurfaceAdapter::GetWindow(Uint32 windowID)
{
	return _windows[windowID];
}



/*static NSString* EventTypeToString(Uint32 type)
{
	switch (type)
	{
		case SDL_QUIT: return @"SDL_QUIT";
		case SDL_APP_TERMINATING: return @"SDL_APP_TERMINATING";
		case SDL_APP_LOWMEMORY: return @"SDL_APP_LOWMEMORY";
		case SDL_APP_WILLENTERBACKGROUND: return @"SDL_APP_WILLENTERBACKGROUND";
		case SDL_APP_DIDENTERBACKGROUND: return @"SDL_APP_DIDENTERBACKGROUND";
		case SDL_APP_WILLENTERFOREGROUND: return @"SDL_APP_WILLENTERFOREGROUND";
		case SDL_APP_DIDENTERFOREGROUND: return @"SDL_APP_DIDENTERFOREGROUND";
		case SDL_WINDOWEVENT: return @"SDL_WINDOWEVENT";
		case SDL_SYSWMEVENT: return @"SDL_SYSWMEVENT";
		case SDL_KEYDOWN: return @"SDL_KEYDOWN";
		case SDL_KEYUP: return @"SDL_KEYUP";
		case SDL_TEXTEDITING: return @"SDL_TEXTEDITING";
		case SDL_TEXTINPUT: return @"SDL_TEXTINPUT";
		case SDL_MOUSEMOTION: return @"SDL_MOUSEMOTION";
		case SDL_MOUSEBUTTONDOWN: return @"SDL_MOUSEBUTTONDOWN";
		case SDL_MOUSEBUTTONUP: return @"SDL_MOUSEBUTTONUP";
		case SDL_MOUSEWHEEL: return @"SDL_MOUSEWHEEL";
		case SDL_JOYAXISMOTION: return @"SDL_JOYAXISMOTION";
		case SDL_JOYBALLMOTION: return @"SDL_JOYBALLMOTION";
		case SDL_JOYHATMOTION: return @"SDL_JOYHATMOTION";
		case SDL_JOYBUTTONDOWN: return @"SDL_JOYBUTTONDOWN";
		case SDL_JOYBUTTONUP: return @"SDL_JOYBUTTONUP";
		case SDL_JOYDEVICEADDED: return @"SDL_JOYDEVICEADDED";
		case SDL_JOYDEVICEREMOVED: return @"SDL_JOYDEVICEREMOVED";
		case SDL_CONTROLLERAXISMOTION: return @"SDL_CONTROLLERAXISMOTION";
		case SDL_CONTROLLERBUTTONDOWN: return @"SDL_CONTROLLERBUTTONDOWN";
		case SDL_CONTROLLERBUTTONUP: return @"SDL_CONTROLLERBUTTONUP";
		case SDL_CONTROLLERDEVICEADDED: return @"SDL_CONTROLLERDEVICEADDED";
		case SDL_CONTROLLERDEVICEREMOVED: return @"SDL_CONTROLLERDEVICEREMOVED";
		case SDL_CONTROLLERDEVICEREMAPPED: return @"SDL_CONTROLLERDEVICEREMAPPED";
		case SDL_FINGERDOWN: return @"SDL_FINGERDOWN";
		case SDL_FINGERUP: return @"SDL_FINGERUP";
		case SDL_FINGERMOTION: return @"SDL_FINGERMOTION";
		case SDL_DOLLARGESTURE: return @"SDL_DOLLARGESTURE";
		case SDL_DOLLARRECORD: return @"SDL_DOLLARRECORD";
		case SDL_MULTIGESTURE: return @"SDL_MULTIGESTURE";
		case SDL_CLIPBOARDUPDATE: return @"SDL_CLIPBOARDUPDATE";
		case SDL_DROPFILE: return @"SDL_DROPFILE";
		case SDL_USEREVENT: return @"SDL_USEREVENT";
		default: return @"[event?]";
	}
}*/



void SurfaceAdapter::ProcessEvent(const SDL_Event& event)
{
	//NSLog(@"ProcessEvent %@", EventTypeToString(event.type));

	SurfaceAdapter* window;
	switch (event.type)
	{
		case SDL_QUIT:
			_done = true;
			break;

		case SDL_WINDOWEVENT:
			window = GetWindow(event.window.windowID);
			if (window) window->ProcessWindow(event.window);
			break;

		case SDL_KEYDOWN:
			window = GetWindow(event.key.windowID);
			if (window) window->ProcessKeyDown(event.key);
			break;

		case SDL_KEYUP:
			window = GetWindow(event.key.windowID);
			if (window) window->ProcessKeyUp(event.key);
			break;

		case SDL_FINGERDOWN:
			window = _touchWindow;
			if (window) window->ProcessFingerDown(event.tfinger);
			break;

		case SDL_FINGERUP:
			window = _touchWindow;
			if (window) window->ProcessFingerUp(event.tfinger);
			break;

		case SDL_FINGERMOTION:
			window = _touchWindow;
			if (window) window->ProcessFingerMotion(event.tfinger);
			break;

		case SDL_MOUSEMOTION:
			window = GetWindow(event.motion.windowID);
			if (window && event.motion.which != SDL_TOUCH_MOUSEID) window->ProcessMouseMotion(event.motion);
			break;

		case SDL_MOUSEBUTTONDOWN:
			window = GetWindow(event.button.windowID);
			if (window && event.button.which != SDL_TOUCH_MOUSEID) window->ProcessMouseButtonDown(event.button);
			break;

		case SDL_MOUSEBUTTONUP:
			window = GetWindow(event.button.windowID);
			if (window && event.button.which != SDL_TOUCH_MOUSEID) window->ProcessMouseButtonUp(event.button);
			break;

		case SDL_MOUSEWHEEL:
			window = GetWindow(event.wheel.windowID);
			if (window && event.wheel.which != SDL_TOUCH_MOUSEID) window->ProcessMouseWheel(event.wheel);
			break;
	}
}


void SurfaceAdapter::ProcessWindow(const SDL_WindowEvent& event)
{
	int x, y, w, h;
	SDL_GetWindowPosition(_window, &x, &y);
	SDL_GetWindowSize(_window, &w, &h);

	_surface->SetNativeSize(glm::vec2(w, h));
}


static char TranslateKeyCode(const SDL_Keysym& keysym)
{
	switch (keysym.sym)
	{
		case SDLK_q: return 'Q';
		case SDLK_w: return 'W';
		case SDLK_e: return 'E';
		case SDLK_a: return 'A';
		case SDLK_s: return 'S';
		case SDLK_d: return 'D';
		case SDLK_1: return '1';
		case SDLK_2: return '2';
		case SDLK_3: return '3';
		case SDLK_4: return '4';
		case SDLK_5: return '5';
		case SDLK_6: return '6';
		case SDLK_7: return '7';
		case SDLK_8: return '8';
		case SDLK_9: return '9';
		default: return '\0';
	}
}


void SurfaceAdapter::ProcessKeyDown(const SDL_KeyboardEvent& event)
{
	char key = TranslateKeyCode(event.keysym);
	if (key == '\0')
		return;

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyDown(key);

}


void SurfaceAdapter::ProcessKeyUp(const SDL_KeyboardEvent& event)
{
	char key = TranslateKeyCode(event.keysym);
	if (key == '\0')
		return;

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyUp(key);
}


void SurfaceAdapter::ProcessFingerDown(const SDL_TouchFingerEvent& event)
{
	if (_surface == nullptr)
		return;

	glm::vec2 position = ToPosition(event);
	double timestamp = ToTimestamp(event.timestamp);

	Touch* touch = new Touch(1, position, timestamp, MouseButtons());
	_touches[MakeTouchKey(event)] = touch;
	_surface->NotifyViewsOfTouchBegin(touch);

	touch->TouchBegin();
	touch->TouchBegan();
}


void SurfaceAdapter::ProcessFingerUp(const SDL_TouchFingerEvent& event)
{
	if (_surface == nullptr)
		return;

	auto i = _touches.find(MakeTouchKey(event));
	if (i != _touches.end())
	{
		Touch* touch = i->second;

		glm::vec2 position = ToPosition(event);
		glm::vec2 previous = touch->GetCurrentPosition();
		double timestamp = ToTimestamp(event.timestamp);
		touch->Update(position, previous, timestamp);

		touch->TouchEnded();

		_touches.erase(MakeTouchKey(event));
		delete touch;
	}
}


void SurfaceAdapter::ProcessFingerMotion(const SDL_TouchFingerEvent& event)
{
	if (_surface == nullptr)
		return;

	auto i = _touches.find(MakeTouchKey(event));
	if (i != _touches.end())
	{
		Touch* touch = i->second;

		glm::vec2 position = ToPosition(event);
		glm::vec2 previous = touch->GetCurrentPosition();
		if (position != previous)
		{
			double timestamp = ToTimestamp(event.timestamp);
			touch->Update(position, previous, timestamp);

			touch->TouchMoved();
		}
	}
}



void SurfaceAdapter::ProcessMouseMotion(const SDL_MouseMotionEvent& event)
{
	if (_mouseTouch)
	{
		glm::vec2 position = ToVector(event.x, event.y);
		double timestamp = ToTimestamp(event.timestamp);

		MouseButtons buttons = _mouseTouch->GetCurrentButtons();
		buttons.left = (event.state & SDL_BUTTON_LMASK) != 0;
		buttons.right = (event.state & SDL_BUTTON_RMASK) != 0;
		buttons.other = (event.state & SDL_BUTTON_MMASK) != 0;

		_mouseTouch->Update(position, timestamp, buttons);

        if (_mouseTouch->GetCurrentButtons().Any())
			_mouseTouch->TouchMoved();
	}
}


void SurfaceAdapter::ProcessMouseButtonDown(const SDL_MouseButtonEvent& event)
{
	//NSLog(@"ProcessMouseButtonDown button=%d state=%d", event.button, event.state);

	glm::vec2 position = ToVector(event.x, event.y);
	double timestamp = ToTimestamp(event.timestamp);

	MouseButtons buttons;
	if (_mouseTouch)
		buttons = _mouseTouch->GetCurrentButtons();

	switch (event.button)
	{
		case SDL_BUTTON_LEFT: buttons.left = true; break;
		case SDL_BUTTON_RIGHT: buttons.right = true; break;
		case SDL_BUTTON_MIDDLE: buttons.other = true; break;
	}

	if (_mouseTouch == nullptr)
	{
		_mouseTouch = new Touch(1, position, timestamp, buttons);
		_surface->NotifyViewsOfTouchBegin(_mouseTouch);
	}
	else
	{
		_mouseTouch->Update(position, timestamp, buttons);
	}

	_mouseTouch->TouchBegin();
	_mouseTouch->TouchBegan();
}


void SurfaceAdapter::ProcessMouseButtonUp(const SDL_MouseButtonEvent& event)
{
	//NSLog(@"ProcessMouseButtonUp button=%d state=%d", event.button, event.state);

	if (_mouseTouch)
	{
		glm::vec2 position = ToVector(event.x, event.y);
		double timestamp = ToTimestamp(event.timestamp);

		MouseButtons buttons = _mouseTouch->GetCurrentButtons();
		switch (event.button)
		{
			case SDL_BUTTON_LEFT: buttons.left = false; break;
			case SDL_BUTTON_RIGHT: buttons.right = false; break;
			case SDL_BUTTON_MIDDLE: buttons.other = false; break;
		}

		_mouseTouch->Update(position, timestamp, buttons);

		if (buttons.Any())
		{
			_mouseTouch->TouchMoved();
		}
		else
		{
			_mouseTouch->TouchEnded();
			delete _mouseTouch;
			_mouseTouch = nullptr;
		}
	}
}


void SurfaceAdapter::ProcessMouseWheel(const SDL_MouseWheelEvent& event)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	glm::vec2 position = ToVector(x, y);
    glm::vec2 delta = 0.01f * glm::vec2(event.x, event.y);

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->ScrollWheel(position, delta);

}


void SurfaceAdapter::Update()
{
	_surface->SetNativeSize((glm::vec2)GetWindowSize());

	std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
	double secondsSinceTimeStart = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - _timestart).count();
	_timestamp = timestamp;

	AnimationHost::Tick();

	if (_mouseTouch)
	{
		double oldTimestamp = _mouseTouch->GetTimestamp();
		_mouseTouch->Update(secondsSinceTimeStart);

		if (_mouseTouch->GetTimestamp() != oldTimestamp)
			_mouseTouch->TouchMoved();
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


glm::vec2 SurfaceAdapter::ToVector(int x, int y)
{
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	return glm::vec2(x, h - y);
}


glm::ivec2 SurfaceAdapter::GetWindowSize() const
{
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	return glm::ivec2(w, h);
}


glm::vec2 SurfaceAdapter::ToPosition(const SDL_TouchFingerEvent& event)
{
	if (_surface == nullptr)
		return glm::vec2();

	return _surface->GetVirtualSize() * glm::vec2(event.x, 1.0f - event.y);
}


double SurfaceAdapter::ToTimestamp(Uint32 timestamp)
{
	// TODO: calculate correct timestamp
	return 0.000001 * std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - _timestart).count();
}


std::pair<SDL_TouchID, SDL_FingerID> SurfaceAdapter::MakeTouchKey(const SDL_TouchFingerEvent& event)
{
	return std::pair<SDL_TouchID, SDL_FingerID>(event.touchId, event.fingerId);
}
