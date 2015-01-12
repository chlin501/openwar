// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Hotspot_H
#define Hotspot_H

#include <vector>

class Gesture;
class Surface;
class Touch;


class Hotspot
{
	friend class Touch;
	std::vector<Touch*> _subscribedTouches;
	std::vector<Touch*> _capturedTouches;

public:
	Hotspot();
	virtual ~Hotspot();

	Hotspot(const Hotspot&) = delete;
	Hotspot& operator=(const Hotspot&) = delete;

	virtual Gesture* GetGesture() = 0;

	void SubscribeSurface(Surface* surface);
	void UnsubscribeSurface(Surface* surface);

	void SubscribeTouch(Touch* touch);
	void UnsubscribeTouch(Touch* touch);

	bool TryCaptureTouch(Touch* touch);
	void ReleaseTouch(Touch* touch);

	bool HasCapturedTouch(Touch* touch) const;
	bool HasCapturedTouch() const;
	bool HasCapturedTouches() const;

	Touch* GetCapturedTouch() const;
	const std::vector<Touch*>& GetCapturedTouches() const;
	const std::vector<Touch*>& GetSubscribedTouches() const;
};


#endif
