// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Touch.h"
#include "Gesture.h"
#include "Surface.h"



Touch::Touch(Surface* surface, int tapCount, glm::vec2 position, double timestamp, MouseButtons buttons) :
_surface(surface),
_gestures(),
_tapCount(tapCount),
_hasMoved(false),
_position(position),
_previous(position),
_original(position),
_timestart(timestamp),
_timestamp(timestamp),
_currentButtons(buttons),
_previousButtons()
{
	_sampler.add(timestamp, position);
	UpdateHotspots();
}


Touch::~Touch()
{
	for (Gesture* gesture : _gestures)
	{
		gesture->_touches.erase(
			std::remove(gesture->_touches.begin(), gesture->_touches.end(), this),
			gesture->_touches.end());
	}
}


void Touch::UpdateHotspots()
{
	_hotspots.clear();
	_surface->FindHotspots(glm::mat4(), _position, this);
}


void Touch::AddHotspot(std::shared_ptr<Hotspot> hotspot)
{
	_hotspots.push_back(hotspot);
}


const std::vector<std::shared_ptr<Hotspot>>& Touch::GetHotspots() const
{
	return _hotspots;
}


int Touch::GetTapCount() const
{
	return _tapCount;
}


Surface* Touch::GetSurface() const
{
	return _surface;
}


const std::vector<Gesture*>& Touch::GetGestures() const
{
	return _gestures;
}


bool Touch::HasGesture() const
{
	return !_gestures.empty();
}


void Touch::Update(glm::vec2 position, glm::vec2 previous, double timestamp)
{
	_timestamp = timestamp;
	_previous = previous;
	_position = position;

	_sampler.add(timestamp, position);

	if (GetMotion() == Motion::Moving)
		_hasMoved = true;

	UpdateHotspots();
}


void Touch::Update(glm::vec2 position, double timestamp, MouseButtons buttons)
{
	_timestamp = timestamp;
	_previous = _position;
	_position = position;

	_previousButtons = _currentButtons;
	_currentButtons = buttons;

	_sampler.add(timestamp, position);

	if (GetMotion() == Motion::Moving)
		_hasMoved = true;

	UpdateHotspots();
}


void Touch::Update(double timestamp)
{
	if (timestamp - _timestamp > 0.15)
	{
		_previous = _position;
		_timestamp = timestamp;
	}
}


glm::vec2 Touch::GetPosition() const
{
	return _position;
}


glm::vec2 Touch::GetPrevious() const
{
	return _previous;
}


glm::vec2 Touch::GetOriginal() const
{
	return _original;
}


double Touch::GetTimeStart() const
{
	return _timestart;
}


double Touch::GetTimestamp() const
{
	return _timestamp;
}


MouseButtons Touch::GetCurrentButtons() const
{
	return _currentButtons;
}


MouseButtons Touch::GetPreviousButtons() const
{
	return _previousButtons;
}


Motion Touch::GetMotion() const
{
	if (_timestamp - _sampler.time() > 0.15)
		return Motion::Stationary;

	float speed = glm::length(GetVelocity(_timestamp));

	if (speed > 50)
		return Motion::Moving;

	if (_timestamp - _timestart < 0.2)
		return Motion::Unknown;

	if (speed < 1)
		return Motion::Stationary;

	return Motion::Unknown;
}


bool Touch::HasMoved() const
{
	return _hasMoved;
}


void Touch::ResetHasMoved()
{
	_hasMoved = false;
	_original = _position;
}


void Touch::ResetVelocity()
{
	_sampler.clear();
	_sampler.add(_timestamp, _position);
}


glm::vec2 Touch::GetVelocity() const
{
	return GetVelocity(_timestamp);
}


glm::vec2 Touch::GetVelocity(double timestamp) const
{
	float dt = 0.1f;
	glm::vec2 p1 = _sampler.get(timestamp - dt);
	glm::vec2 p2 = _sampler.get(timestamp);

	return (p2 - p1) / dt;
}
