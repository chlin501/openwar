// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TerrainGesture.h"
#include "TerrainView.h"
#include "TerrainHotspot.h"
#include "TerrainViewport.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/CommonShaders.h"
#include "Surface/Surface.h"
#include "Surface/Touch.h"
#include <glm/gtc/constants.hpp>


TerrainGesture::TerrainGesture(TerrainHotspot* hotspot) :
	_hotspot{hotspot}
{
}


TerrainGesture::~TerrainGesture()
{
}


void TerrainGesture::OnRenderLoop(double secondsSinceLastUpdate)
{
	if (!_hotspot->HasCapturedTouches())
	{
		UpdateMomentumOrbit(secondsSinceLastUpdate);
		UpdateMomentumScroll(secondsSinceLastUpdate);
	}
	else
	{
		_orbitVelocity = 0;
		_scrollVelocity = glm::vec2();
		//_keyOrbitMomentum = 0;
		//_keyScrollMomentum = glm::vec2();
	}

	UpdateKeyOrbit(secondsSinceLastUpdate);
	UpdateKeyScroll(secondsSinceLastUpdate);
}


void TerrainGesture::KeyDown(char key)
{
	switch (key)
	{
		case 'W': _keyScrollForward = true; break;
		case 'A': _keyScrollLeft = true; break;
		case 'S': _keyScrollBackward = true; break;
		case 'D': _keyScrollRight = true; break;
		case 'Q': _keyOrbitRight = true; break;
		case 'E': _keyOrbitLeft = true; break;
		default: break;
	}
}


void TerrainGesture::KeyUp(char key)
{
	switch (key)
	{
		case 'W': _keyScrollForward = false; break;
		case 'A': _keyScrollLeft = false; break;
		case 'S': _keyScrollBackward = false; break;
		case 'D': _keyScrollRight = false; break;
		case 'Q': _keyOrbitRight = false; break;
		case 'E': _keyOrbitLeft = false; break;
		default: break;
	}
}


void TerrainGesture::ScrollWheel(glm::vec2 position, glm::vec2 delta)
{
	Magnify(position, -delta.y / 10);
}


void TerrainGesture::Magnify(glm::vec2 position, float magnification)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();
	glm::vec2 p = (glm::vec2)terrainView->GetViewport()->GetBounds().mid();
	glm::vec2 d1 = glm::vec2(0, 64);
	glm::vec2 d2 = d1 * glm::exp(magnification);

	auto contentPositions = std::make_pair(
		terrainView->GetTerrainPosition3(p - d1),
		terrainView->GetTerrainPosition3(p + d1));

	auto screenPositions = std::make_pair(p - d2, p + d2);

	terrainView->Zoom(contentPositions, screenPositions);
}


void TerrainGesture::TouchWasCaptured(Touch* touch)
{
}


void TerrainGesture::TouchWillBeReleased(Touch* touch)
{
}


void TerrainGesture::TouchBegan(Touch* touch)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();
	bounds2f viewportBounds = (bounds2f)terrainView->GetViewport()->GetBounds();
	if (!viewportBounds.contains(touch->GetCurrentPosition()))
		return;

	if (_hotspot->TryCaptureTouch(touch))
	{
		ResetSamples(touch->GetTimestamp());
	}
}


void TerrainGesture::TouchMoved(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		TerrainView* terrainView = _hotspot->GetTerrainView();

		switch (_hotspot->GetTerrainHotspotMode())
		{
			case TerrainHotspotMode::Move:
				terrainView->Orbit(
					_hotspot->GetOrbitAnchor(),
					CalculateOrbitFactor() * _hotspot->GetOrbitAngle());

				terrainView->Move(
					_hotspot->GetContentPosition1(),
					_hotspot->GetCurrentScreenPosition1());
				break;

			case TerrainHotspotMode::Zoom:
				terrainView->Zoom(
					_hotspot->GetContentPositions(),
					_hotspot->GetCurrentScreenPositions());
				break;

			default:
				break;
		}

		AdjustToKeepInView(0.5, 0);
		UpdateSamples(touch->GetTimestamp());
	}
}


void TerrainGesture::TouchEnded(Touch* touch)
{
	if (_hotspot->HasCapturedTouch(touch))
	{
		if (_hotspot->GetCapturedTouches().size() == 1)
		{
			_scrollVelocity = GetScrollVelocity();
			_orbitVelocity = GetOrbitVelocity();
		}
		else
		{
			ResetSamples(touch->GetTimestamp());
		}
	}
}


void TerrainGesture::UpdateMomentumOrbit(double secondsSinceLastUpdate)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();
	glm::vec2 screenPosition = terrainView->GetViewport()->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec3 contentAnchor = terrainView->GetTerrainPosition2(screenPosition);

	terrainView->Orbit(contentAnchor, (float)secondsSinceLastUpdate * _orbitVelocity);
	_orbitVelocity = _orbitVelocity * exp2f(-4 * (float)secondsSinceLastUpdate);
}


void TerrainGesture::UpdateMomentumScroll(double secondsSinceLastUpdate)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();
	glm::vec2 screenPosition = terrainView->GetViewport()->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec3 contentPosition = terrainView->GetTerrainPosition2(screenPosition);

	contentPosition += (float)secondsSinceLastUpdate * glm::vec3(_scrollVelocity, 0);
	terrainView->Move(contentPosition, screenPosition);

	_scrollVelocity = _scrollVelocity * exp2f(-4 * (float)secondsSinceLastUpdate);

	AdjustToKeepInView(0.35f, (float)secondsSinceLastUpdate);
}


void TerrainGesture::UpdateKeyScroll(double secondsSinceLastUpdate)
{
	float limit = 40;
	if (_keyScrollLeft) _keyScrollMomentum.y += limit;
	if (_keyScrollRight) _keyScrollMomentum.y -= limit;
	if (_keyScrollForward) _keyScrollMomentum.x += limit;
	if (_keyScrollBackward) _keyScrollMomentum.x -= limit;

	TerrainView* terrainView = _hotspot->GetTerrainView();
	glm::vec3 pos = terrainView->GetViewport()->GetCameraPosition();
	glm::vec3 dir = terrainView->GetViewport()->GetCameraDirection();
	glm::vec2 delta = (float)secondsSinceLastUpdate * glm::log(2.0f + glm::max(0.0f, pos.z)) * rotate(_keyScrollMomentum, angle(dir.xy()));
	terrainView->MoveCamera(pos + glm::vec3(delta, 0));

	_keyScrollMomentum *= exp2f(-25 * (float)secondsSinceLastUpdate);
}


void TerrainGesture::UpdateKeyOrbit(double secondsSinceLastUpdate)
{
	if (_keyOrbitLeft) _keyOrbitMomentum -= 32 * (float)secondsSinceLastUpdate;
	if (_keyOrbitRight) _keyOrbitMomentum += 32 * (float)secondsSinceLastUpdate;

	TerrainView* terrainView = _hotspot->GetTerrainView();
	glm::vec2 centerScreen = terrainView->GetViewport()->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec3 contentAnchor = terrainView->GetTerrainPosition3(centerScreen);
	terrainView->Orbit(contentAnchor, (float)secondsSinceLastUpdate * _keyOrbitMomentum);

	_keyOrbitMomentum *= exp2f(-25 * (float)secondsSinceLastUpdate);
}


void TerrainGesture::ResetSamples(double timestamp)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();

	_previousCameraDirection = angle(terrainView->GetViewport()->GetCameraDirection().xy());
	_orbitAccumulator = 0;

	glm::vec2 screenPosition = terrainView->GetViewport()->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec3 contentPosition = terrainView->GetTerrainPosition2(screenPosition);

	_scrollSampler.clear();
	_scrollSampler.add(timestamp, contentPosition.xy());
	_orbitSampler.clear();
	_orbitSampler.add(timestamp, glm::vec2(_orbitAccumulator, 0));
}


void TerrainGesture::UpdateSamples(double timestamp)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();

	float currentCameraDirection = angle(terrainView->GetViewport()->GetCameraDirection().xy());
	float orbitDelta = diff_radians(currentCameraDirection, _previousCameraDirection);

	_previousCameraDirection = currentCameraDirection;

	_orbitAccumulator += orbitDelta;
	glm::vec2 screenPosition = terrainView->GetViewport()->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec3 contentPosition = terrainView->GetTerrainPosition2(screenPosition);

	_scrollSampler.add(timestamp, contentPosition.xy());
	_orbitSampler.add(timestamp, glm::vec2(_orbitAccumulator, 0));
}


glm::vec2 TerrainGesture::GetScrollVelocity() const
{
	double time = _scrollSampler.time();
	double dt = 0.1;
	glm::vec2 p2 = _scrollSampler.get(time);
	glm::vec2 p1 = _scrollSampler.get(time - dt);
	return (p2 - p1) / (float)dt;
}


float TerrainGesture::GetOrbitVelocity() const
{
	double time = _orbitSampler.time();
	double dt = 0.1;
	float v2 = _orbitSampler.get(time).x;
	float v1 = _orbitSampler.get(time - dt).x;

	return (float)((v2 - v1) / dt);
}


float TerrainGesture::CalculateOrbitFactor() const
{
	float orbitSpeed = glm::abs(GetOrbitVelocity());
	float scrollSpeed = NormalizeScrollSpeed(glm::length(GetScrollVelocity()));

	float orbitFactor = bounds1f{0, 1}.clamp(orbitSpeed * 0.8f);
	float scrollFactor = bounds1f{0, 1}.clamp(scrollSpeed * 6.0f);
	float combinedFactor = bounds1f{0, 1}.clamp(1.0f + orbitFactor - scrollFactor);

	return combinedFactor * _hotspot->GetOrbitFactor();
};


float TerrainGesture::NormalizeScrollSpeed(float value) const
{
	TerrainView* terrainView = _hotspot->GetTerrainView();

	glm::vec2 screenCenter = terrainView->GetViewport()->NormalizedToLocal(glm::vec2{});
	glm::vec3 contentCenter = terrainView->GetTerrainPosition3(screenCenter);
	float angle = terrainView->GetViewport()->GetCameraFacing() + 0.5f * glm::pi<float>();
	glm::vec3 contentPos = contentCenter + glm::vec3{value * vector2_from_angle(angle), 0};
	glm::vec2 screenPos = terrainView->ContentToScreen(contentPos);

	glm::ivec2 viewportSize = terrainView->GetViewport()->GetBounds().size();
	float viewportScale = glm::max(viewportSize.x, viewportSize.y);

	return glm::distance(screenCenter, screenPos) / viewportScale;
};


void TerrainGesture::AdjustToKeepInView(float adjustmentFactor, float secondsSinceLastUpdate)
{
	TerrainView* terrainView = _hotspot->GetTerrainView();

	terrainView->ClampCameraPosition();

#if false

	bool is_scrolling = glm::length(_scrollVelocity) > 16;
	bool brake_scrolling = false;

	bounds2f viewportBounds = terrainView->GetViewport()->GetBounds();
	glm::vec2 left = terrainView->GetScreenLeft();
	glm::vec2 right = terrainView->GetScreenRight();
	float dx1 = left.x - viewportBounds.min.x;
	float dx2 = viewportBounds.max.x - right.x;

	if (dx1 > 0 || dx2 > 0)
	{
		if (is_scrolling)
		{
			brake_scrolling = true;
		}
		else if (dx1 > 0 && dx2 > 0)
		{
			glm::vec3 contentPosition1 = terrainView->GetTerrainPosition2(left);
			glm::vec3 contentPosition2 = terrainView->GetTerrainPosition2(right);
			left.x -= adjustmentFactor * dx1;
			right.x += adjustmentFactor * dx2;
			terrainView->Zoom(contentPosition1, contentPosition2, left, right);
		}
		else if (dx1 > 0)
		{
			glm::vec3 contentPosition = terrainView->GetTerrainPosition2(left);
			left.x -= adjustmentFactor * dx1;
			terrainView->Move(contentPosition, left);
		}
		else if (dx2 > 0)
		{
			glm::vec3 contentPosition = terrainView->GetTerrainPosition2(right);
			right.x += adjustmentFactor * dx2;
			terrainView->Move(contentPosition, right);
		}
	}

	glm::vec2 bottom = terrainView->GetScreenBottom();
	glm::vec2 top = terrainView->GetScreenTop();
	float dy1 = bottom.y - viewportBounds.min.y;
	float dy2 = viewportBounds.min.y + 0.85f * viewportBounds.max.y - top.y;

	if (dy1 > 0 || dy2 > 0)
	{
		if (is_scrolling)
		{
			brake_scrolling = true;
		}
		else if (dy1 > 0 && dy2 > 0)
		{
			glm::vec3 contentPosition1 = terrainView->GetTerrainPosition2(bottom);
			glm::vec3 contentPosition2 = terrainView->GetTerrainPosition2(top);
			bottom.y -= adjustmentFactor * dy1;
			top.y += adjustmentFactor * dy2;
			terrainView->Zoom(contentPosition1, contentPosition2, bottom, top);
		}
		else if (dy1 > 0)
		{
			glm::vec3 contentPosition = terrainView->GetTerrainPosition2(bottom);
			bottom.y -= adjustmentFactor * dy1;
			terrainView->Move(contentPosition, bottom);
		}
		else if (dy2 > 0)
		{
			glm::vec3 contentPosition = terrainView->GetTerrainPosition2(top);
			top.y += adjustmentFactor * dy2;
			terrainView->Move(contentPosition, top);
		}
	}

	if (brake_scrolling)
	{
		_scrollVelocity = _scrollVelocity * exp2f(-16 * secondsSinceLastUpdate);
	}

#endif
}
