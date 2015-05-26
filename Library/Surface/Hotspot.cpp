// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Hotspot.h"

#include "Gesture.h"
#include "Surface.h"
#include "Touch.h"


Hotspot::Hotspot()
{
}


Hotspot::~Hotspot()
{
	for (Touch* touch : _subscribedTouches)
	{
		touch->_subscribedHotspots.erase(
			std::remove(touch->_subscribedHotspots.begin(), touch->_subscribedHotspots.end(), this),
			touch->_subscribedHotspots.end());
	}

	while (!_capturedTouches.empty())
		ReleaseTouch(_capturedTouches.back());

	while (!_subscribedTouches.empty())
		UnsubscribeTouch(_subscribedTouches.back());
}


bool Hotspot::HasCapturedTouch(Touch* touch) const
{
	return std::find(_capturedTouches.begin(), _capturedTouches.end(), touch) != _capturedTouches.end();
}


bool Hotspot::HasCapturedTouch() const
{
	return _capturedTouches.size() == 1;
}


bool Hotspot::HasCapturedTouches() const
{
	return !_capturedTouches.empty();
}


Touch* Hotspot::GetCapturedTouch() const
{
	return _capturedTouches.size() == 1 ? _capturedTouches.front() : nullptr;
}


const std::vector<Touch*>& Hotspot::GetCapturedTouches() const
{
	return _capturedTouches;
}


const std::vector<Touch*>& Hotspot::GetSubscribedTouches() const
{
	return _subscribedTouches;
}


void Hotspot::SubscribeSurface(Surface* surface)
{
}


void Hotspot::UnsubscribeSurface(Surface* surface)
{
}


void Hotspot::SubscribeTouch(Touch* touch)
{
	if (std::find(_subscribedTouches.begin(), _subscribedTouches.end(), touch) != _subscribedTouches.end())
		return;

	_subscribedTouches.push_back(touch);
	touch->_subscribedHotspots.push_back(this);
}


void Hotspot::UnsubscribeTouch(Touch* touch)
{
	if (HasCapturedTouch(touch))
		ReleaseTouch(touch);

	_subscribedTouches.erase(
		std::remove(_subscribedTouches.begin(), _subscribedTouches.end(), touch),
		_subscribedTouches.end());

	touch->_subscribedHotspots.erase(
		std::remove(touch->_subscribedHotspots.begin(), touch->_subscribedHotspots.end(), this),
		touch->_subscribedHotspots.end());
}


bool Hotspot::TryCaptureTouch(Touch* touch)
{
	if (touch->_capturedByHotspot == this)
		return true;

	if (touch->_capturedByHotspot)
	{
		Gesture* gesture = touch->_capturedByHotspot->GetGesture();
		gesture->AskReleaseTouchToAnotherHotspot(touch, this);
		if (touch->_capturedByHotspot)
			return false;
	}

	_capturedTouches.push_back(touch);
	touch->_capturedByHotspot = this;

	std::vector<Hotspot*> hotspots(touch->_subscribedHotspots);
	for (Hotspot* hotspot : hotspots)
		hotspot->GetGesture()->TouchWasCaptured(touch);

	return true;
}


void Hotspot::ReleaseTouch(Touch* touch)
{
	if (!HasCapturedTouch(touch))
		return;

	std::vector<Hotspot*> hotspots(touch->_subscribedHotspots);
	for (Hotspot* hotspot : hotspots)
		hotspot->GetGesture()->TouchWillBeReleased(touch);

	_capturedTouches.erase(
		std::remove(_capturedTouches.begin(), _capturedTouches.end(), touch),
		_capturedTouches.end());

	touch->_capturedByHotspot = nullptr;
}
