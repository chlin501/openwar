// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINGESTURE_H
#define TERRAINGESTURE_H

#include "Gestures/Gesture.h"
#include "Algorithms/sampler.h"

class TerrainView;
class TerrainHotspot;


class TerrainGesture : public Gesture
{
	TerrainHotspot* _hotspot;
	glm::vec3 _contentPosition1;
	glm::vec3 _contentPosition2;
	glm::vec2 _previousTouchPosition;

	sampler _scrollSampler;
	sampler _orbitSampler;
	glm::vec2 _scrollVelocity;
	float _previousCameraDirection;
	float _orbitAccumulator;
	float _orbitVelocity;
	bool _keyScrollLeft;
	bool _keyScrollRight;
	bool _keyScrollForward;
	bool _keyScrollBackward;
	bool _keyOrbitLeft;
	bool _keyOrbitRight;
	float _keyOrbitMomentum;
	glm::vec2 _keyScrollMomentum;

public:
	TerrainGesture(TerrainHotspot* hotspot);
	virtual ~TerrainGesture();

	virtual void Update(double secondsSinceLastUpdate);

	virtual void KeyDown(char key);
	virtual void KeyUp(char key);

	virtual void ScrollWheel(glm::vec2 position, glm::vec2 delta);
	virtual void Magnify(glm::vec2 position, float magnification);

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved();
	virtual void TouchEnded(Touch* touch);

private:
	void UpdateMomentumOrbit(double secondsSinceLastUpdate);
	void UpdateMomentumScroll(double secondsSinceLastUpdate);
	void UpdateKeyScroll(double secondsSinceLastUpdate);
	void UpdateKeyOrbit(double secondsSinceLastUpdate);

	void MoveAndOrbit(Touch* touch);
	void ZoomAndOrbit(Touch* touch1, Touch* touch2);

	void ResetSamples(double timestamp);
	void UpdateSamples(double timestamp);

	glm::vec2 GetScrollVelocity() const;
	float GetOrbitVelocity() const;

	void AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate);
};


#endif
