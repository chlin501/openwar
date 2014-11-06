// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Hotspot_H
#define Hotspot_H

class Gesture;


class HotspotBase
{
public:
	HotspotBase();
	virtual ~HotspotBase();
	virtual Gesture* GetGesture() const = 0;
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
