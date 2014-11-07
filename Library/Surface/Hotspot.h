// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Hotspot_H
#define Hotspot_H

#include <vector>

class Gesture;
class Touch;


class HotspotBase
{
public:
	std::vector<Touch*> _touches;

	HotspotBase();
	virtual ~HotspotBase();
	virtual Gesture* GetGesture() const = 0;

	void CaptureTouch(Touch* touch);
	void ReleaseTouch(Touch* touch);
};


template <class _Hotspot, class _Gesture>
class Hotspot : public HotspotBase
{
	mutable _Gesture* _gesture;

public:
	Hotspot() : _gesture(nullptr) { }
	virtual ~Hotspot() { delete _gesture; }

	virtual Gesture* GetGesture() const
	{
		if (_gesture == nullptr)
		{
			_Hotspot* hotspot = dynamic_cast<_Hotspot*>(const_cast<Hotspot<_Hotspot, _Gesture>*>(this));
			_gesture = new _Gesture(hotspot);
		}
		return _gesture;
	}
};


#endif
