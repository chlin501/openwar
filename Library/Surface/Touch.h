// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TOUCH_H
#define TOUCH_H

#include <vector>
#include "Algorithms/sampler.h"

class Gesture;
class Hotspot;
class Surface;


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

	Surface* _surface;
	std::vector<Gesture*> _gestures;
	int _tapCount;
	bool _hasMoved;
	glm::vec2 _position;
	glm::vec2 _previous;
	glm::vec2 _original;
	double _timestart;
	double _timestamp;
	sampler _sampler;
	MouseButtons _currentButtons;
	MouseButtons _previousButtons;
	std::vector<std::shared_ptr<Hotspot>> _hotspots;
	bool _hasBegun;

public:
	Touch(Surface* surface, int tapCount, glm::vec2 position, double timestamp, MouseButtons buttons);
	~Touch();

	void UpdateHotspots();
	void AddHotspot(std::shared_ptr<Hotspot> hotspot);
	const std::vector<std::shared_ptr<Hotspot>>& GetHotspots() const;

	int GetTapCount() const;

	Surface* GetSurface() const;
	const std::vector<Gesture*>& GetGestures() const;
	bool HasGesture() const;

	void Update(glm::vec2 position, glm::vec2 previous, double timestamp);
	void Update(glm::vec2 position, double timestamp, MouseButtons buttons);
	void Update(double timestamp);

	glm::vec2 GetPosition() const;
	glm::vec2 GetPrevious() const;
	glm::vec2 GetOriginal() const;

	double GetTimeStart() const;
	double GetTimestamp() const;

	MouseButtons GetCurrentButtons() const;
	MouseButtons GetPreviousButtons() const;

	Motion GetMotion() const;

	bool HasBegun() const;

	bool HasMoved() const;
	void ResetHasMoved();

	void ResetVelocity();
	glm::vec2 GetVelocity() const;
	glm::vec2 GetVelocity(double timestamp) const;

};


#endif
