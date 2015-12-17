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
	std::vector<Touch*> _subscribedTouches{};
	std::vector<Touch*> _capturedTouches{};

public:
	Hotspot();
	virtual ~Hotspot();

	Hotspot(const Hotspot&) = delete;
	Hotspot& operator=(const Hotspot&) = delete;

	bool HasCapturedTouch(Touch* touch) const;
	bool HasCapturedTouch() const;
	bool HasCapturedTouches() const;

	Touch* GetCapturedTouch() const;
	int CountCapturedTouches() const;
	const std::vector<Touch*>& GetCapturedTouches() const;
	const std::vector<Touch*>& GetSubscribedTouches() const;

	virtual Gesture* GetGesture() = 0;

	virtual void SubscribeSurface(Surface* surface);
	virtual void UnsubscribeSurface(Surface* surface);

	virtual void SubscribeTouch(Touch* touch);
	virtual void UnsubscribeTouch(Touch* touch);

	virtual bool TryCaptureTouch(Touch* touch);
	virtual void ReleaseTouch(Touch* touch);
};


#endif
