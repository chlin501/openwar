// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Hotspot_H
#define Hotspot_H

#include <vector>

class Gesture;
class Touch;


class Hotspot : public std::enable_shared_from_this<Hotspot>
{
	friend class Touch;
	std::vector<Touch*> _subscribedTouches;
	std::vector<Touch*> _capturedTouches;

public:
	virtual ~Hotspot();

	virtual Gesture* GetGesture() const = 0;

	void SubscribeTouch(Touch* touch);
	void UnsubscribeTouch(Touch* touch);

	bool CanCaptureTouch(Touch* touch) const;
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
