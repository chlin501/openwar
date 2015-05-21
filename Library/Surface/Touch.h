// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Touch_H
#define Touch_H

#include <memory>
#include <vector>
#include "Algorithms/vec2_sampler.h"

class Gesture;
class Hotspot;


struct MouseButtons
{
	bool left, right, other;
	MouseButtons(bool l = false, bool r = false, bool o = false) : left(l), right(r), other(o) { }
	bool Any() const { return left || right || other; }
};

inline bool operator==(MouseButtons v1, MouseButtons v2) { return v1.left == v2.left && v1.right == v2.right && v1.other == v2.other; }
inline bool operator!=(MouseButtons v1, MouseButtons v2) { return v1.left != v2.left || v1.right != v2.right || v1.other != v2.other; }


enum class Motion { Unknown, Stationary, Moving };


class Touch
{
	friend class Gesture;
	friend class Hotspot;

	std::vector<Hotspot*> _subscribedHotspots;
	Hotspot* _capturedByHotspot;

	int _tapCount;
	bool _hasMoved;
	glm::vec2 _position;
	glm::vec2 _previous;
	glm::vec2 _original;
	double _timestart;
	double _timestamp;
	vec2_sampler _sampler;
	MouseButtons _currentButtons;
	MouseButtons _previousButtons;

public:
	Touch(int tapCount, glm::vec2 position, double timestamp, MouseButtons buttons);
	~Touch();

	Touch(const Touch&) = delete;
	Touch& operator=(const Touch&) = delete;

	bool IsCaptured() const;

	int GetTapCount() const;

	void TouchBegin();
	void TouchBegan();
	void TouchMoved();
	void TouchEnded();

	void Update(glm::vec2 position, glm::vec2 previous, double timestamp);
	void Update(glm::vec2 position, double timestamp, MouseButtons buttons);
	void Update(double timestamp);

	glm::vec2 GetCurrentPosition() const;
	glm::vec2 GetPreviousPosition() const;
	glm::vec2 GetOriginalPosition() const;
	void ResetPosition(glm::vec2 position);

	double GetTimeStart() const;
	double GetTimestamp() const;

	MouseButtons GetCurrentButtons() const;
	MouseButtons GetPreviousButtons() const;

	Motion GetMotion() const;

	bool HasMoved() const;
	void ResetHasMoved();

	void ResetVelocity();
	glm::vec2 GetVelocity() const;
	glm::vec2 GetVelocity(double timestamp) const;
};


#endif
