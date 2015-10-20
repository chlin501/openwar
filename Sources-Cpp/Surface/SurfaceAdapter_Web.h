// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef SurfaceAdapter_Web_H
#define SurfaceAdapter_Web_H

#include <chrono>
#include "Graphics/GraphicsContext.h"
#include "Touch.h"

extern "C" void SurfaceAdapter_KeyDown(char key);
extern "C" void SurfaceAdapter_KeyUp(char key);
extern "C" void SurfaceAdapter_MouseUpdate(float x, float y, int buttons);
extern "C" void SurfaceAdapter_MouseWheel(float x, float y, float dx, float dy);

class Surface;


class SurfaceAdapter
{
	friend void SurfaceAdapter_KeyDown(char);
	friend void SurfaceAdapter_KeyUp(char);
	friend void SurfaceAdapter_Blur();
	friend void SurfaceAdapter_MouseUpdate(float, float, int);
	friend void SurfaceAdapter_MouseCancel();
	friend void SurfaceAdapter_MouseWheel(float, float, float, float);

	static SurfaceAdapter* _surfaceAdapter;

	Surface* _surface{};
	SDL_Window* _window{};
	SDL_GLContext _glcontext{};
	Touch* _mouse{};
	std::chrono::system_clock::time_point _timestart;
	std::chrono::system_clock::time_point _timestamp;

public:
	SurfaceAdapter(const char* title);
	~SurfaceAdapter();

	void SetSurface(Surface* surface);

	static void Tick();

private:
	void Update();
	void Render();

	void KeyDown(char key);
	void KeyUp(char key);

	void MouseUpdate(float x, float y, int buttons);
	void MouseWheel(float x, float y, float dx, float dy);

	glm::ivec2 GetWindowSize() const;
};


#endif
