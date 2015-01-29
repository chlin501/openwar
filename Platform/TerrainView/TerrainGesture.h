// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINGESTURE_H
#define TERRAINGESTURE_H

#include "Surface/Gesture.h"
#include "Graphics/RenderLoopObserver.h"
#include "vec2_sampler.h"

class TerrainView;
class TerrainHotspot;


class TerrainGesture : public Gesture, RenderLoopObserver
{
	enum class MoveAndOrbitLock { None, Move, Orbit };

	TerrainHotspot* _hotspot{};

	vec2_sampler _scrollSampler;
	vec2_sampler _orbitSampler;
	glm::vec2 _scrollVelocity;
	float _previousCameraDirection{};
	float _orbitAccumulator{};
	float _orbitVelocity{};
	bool _keyScrollLeft{};
	bool _keyScrollRight{};
	bool _keyScrollForward{};
	bool _keyScrollBackward{};
	bool _keyOrbitLeft{};
	bool _keyOrbitRight{};
	float _keyOrbitMomentum{};
	glm::vec2 _keyScrollMomentum;

public:
	TerrainGesture(TerrainHotspot* hotspot);
	virtual ~TerrainGesture();

private: // RenderLoopObserver
	virtual void OnRenderLoop(double secondsSinceLastUpdate);

public:
	virtual void KeyDown(char key);
	virtual void KeyUp(char key);

	virtual void ScrollWheel(glm::vec2 position, glm::vec2 delta);
	virtual void Magnify(glm::vec2 position, float magnification);

	virtual void TouchWasCaptured(Touch* touch);
	virtual void TouchWillBeReleased(Touch* touch);

	virtual void TouchBegan(Touch* touch);
	virtual void TouchMoved(Touch* touch);
	virtual void TouchEnded(Touch* touch);

private:
	void UpdateMomentumOrbit(double secondsSinceLastUpdate);
	void UpdateMomentumScroll(double secondsSinceLastUpdate);
	void UpdateKeyScroll(double secondsSinceLastUpdate);
	void UpdateKeyOrbit(double secondsSinceLastUpdate);

	void ResetSamples(double timestamp);
	void UpdateSamples(double timestamp);

	glm::vec2 GetScrollVelocity() const;
	float GetOrbitVelocity() const;
	float CalculateOrbitFactor() const;
	float NormalizeScrollSpeed(float value) const;

	void AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate);
};


#endif
