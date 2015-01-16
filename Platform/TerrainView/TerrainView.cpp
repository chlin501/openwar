// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "BattleModel/HeightMap.h"
#include "Surface/Touch.h"
#include "Surface/Surface.h"
#include "EditorHotspot.h"
#include "EditorModel.h"
#include "TerrainHotspot.h"
#include "TerrainView.h"
#include "TerrainViewport.h"
#include "CommonShaders.h"


TerrainView::TerrainView(Surface* surface) : View(surface)
{
	GraphicsContext* gc = surface->GetGraphicsContext();
	_terrainViewport = new TerrainViewport(gc);
}


TerrainView::~TerrainView()
{
	delete _terrainViewport;
}


TerrainViewport* TerrainView::GetViewport() const
{
	return _terrainViewport;
}


void TerrainView::SetEditorHotspot(std::shared_ptr<EditorHotspot> hotspot)
{
	_editorHotspot = hotspot;
}


void TerrainView::OnTouchEnter(Touch* touch)
{
}


void TerrainView::OnTouchBegin(Touch* touch)
{
	if (_editorHotspot != nullptr)
		_editorHotspot->SubscribeTouch(touch);

	if (_terrainHotspot == nullptr)
		_terrainHotspot = std::make_shared<TerrainHotspot>(this);

	_terrainHotspot->SubscribeTouch(touch);
}


void TerrainView::RenderMouseHint(VertexShape_3f& vertices)
{
	if (_terrainHotspot)
		_terrainHotspot->RenderMouseHint(vertices);
}


void TerrainView::SetHeightMap(HeightMap* heightMap)
{
	_heightMap = heightMap;

	_terrainViewport->SetTerrainBounds(heightMap->GetBounds());

	/*
	glm::vec2 center = _contentBounds.mid();
	glm::vec2 size = _contentBounds.size();

	_cameraPosition = glm::vec3(center - 0.2f * size, 0.2f * size.y);
	_cameraTilt = (float)M_PI_4;
	_cameraFacing = 0;
	MoveCamera(_cameraPosition);*/
}


glm::vec2 TerrainView::GetScreenTop() const
{
	glm::vec2 center = _terrainViewport->GetTerrainBounds().mid();
	float angle = _terrainViewport->GetCameraFacing();
	if (_terrainViewport->GetFlip())
		angle += (float)M_PI;
	float radius = _terrainViewport->GetContentRadius();

	glm::vec2 result = _terrainViewport->NormalizedToLocal(glm::vec2(0, -1));

	result.y = glm::max(result.y, ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle), 0)).y);

	return result;
}


glm::vec2 TerrainView::GetScreenLeft() const
{
	glm::vec2 center = _terrainViewport->GetTerrainBounds().mid();
	float angle = _terrainViewport->GetCameraFacing();
	if (_terrainViewport->GetFlip())
		angle += (float)M_PI;
	float radius = _terrainViewport->GetContentRadius();

	glm::vec2 result = _terrainViewport->NormalizedToLocal(glm::vec2(1, 0));

	int n = 20;
	for (int i = 0; i < n; ++i)
	{
		float a = i * (float)M_PI / n;
		result.x = glm::min(result.x, ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle + a), 0)).x);
	}

	return result;
}


glm::vec2 TerrainView::GetScreenBottom() const
{
	glm::vec2 center = _terrainViewport->GetTerrainBounds().mid();
	float angle = _terrainViewport->GetCameraFacing() + (float)M_PI;
	if (_terrainViewport->GetFlip())
		angle += (float)M_PI;
	float radius = _terrainViewport->GetContentRadius();

	glm::vec2 result = _terrainViewport->NormalizedToLocal(glm::vec2(0, 1));

	result.y = glm::max(result.y, ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle), 0)).y);

	return result;
}


glm::vec2 TerrainView::GetScreenRight() const
{
	glm::vec2 center = _terrainViewport->GetTerrainBounds().mid();
	float angle = _terrainViewport->GetCameraFacing();
	if (_terrainViewport->GetFlip())
		angle += (float)M_PI;
	float radius = _terrainViewport->GetContentRadius();

	glm::vec2 result = _terrainViewport->NormalizedToLocal(glm::vec2(-1, 0));

	int n = 20;
	for (int i = 4; i < n - 4; ++i)
	{
		float a = i * (float)M_PI / n;
		result.x = glm::max(result.x, ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle - a), 0)).x);
	}

	return result;
}



ray TerrainView::GetCameraRay(glm::vec2 screenPosition) const
{
	glm::vec2 viewPosition = _terrainViewport->LocalToNormalized(screenPosition);
	glm::mat4 inverse = glm::inverse(_terrainViewport->GetTransform());
	glm::vec4 p1 = inverse * glm::vec4(viewPosition, 0, 1.0f);
	glm::vec4 p2 = inverse * glm::vec4(viewPosition, 0.5f, 1.0f);

	glm::vec3 q1 = (glm::vec3)p1.xyz() / p1.w;
	glm::vec3 q2 = (glm::vec3)p2.xyz() / p2.w;

	glm::vec3 direction =  glm::normalize(q2 - q1);
	glm::vec3 origin = q1 - 200.0f * direction;
	return ray(origin, direction);
}


glm::vec3 TerrainView::GetTerrainPosition2(glm::vec2 screenPosition, float height) const
{
	ray r = GetCameraRay(screenPosition);
	std::pair<bool, float> d = intersect(r, plane(glm::vec3(0, 0, 1), height));
	return r.point(d.first ? d.second : 0);
}


glm::vec3 TerrainView::GetTerrainPosition3(glm::vec2 screenPosition) const
{
	if (_heightMap == nullptr)
		return glm::vec3();

	ray r = GetCameraRay(screenPosition);
	std::pair<bool, float> d = _heightMap->Intersect(r);
	return r.point(d.first ? d.second : 0);
}


void TerrainView::Move(glm::vec3 originalContentPosition, glm::vec2 currentScreenPosition)
{
	ray ray1 = GetCameraRay(currentScreenPosition);
	ray ray2 = ray(originalContentPosition, -ray1.direction);

	plane cameraPlane(glm::vec3(0, 0, 1), _terrainViewport->GetCameraPosition());
	std::pair<bool, float> d = intersect(ray2, cameraPlane);
	if (d.first)
	{
		MoveCamera(ray2.point(d.second));
	}
}


void TerrainView::Zoom(std::pair<glm::vec3, glm::vec3> originalContentPositions, std::pair<glm::vec2, glm::vec2> currentScreenPositions)
{
	glm::vec3 contentAnchor = (originalContentPositions.first + originalContentPositions.second) / 2.0f;
	glm::vec2 currentScreenCenter = (currentScreenPositions.first + currentScreenPositions.second) / 2.0f;

	float delta = glm::length(_terrainViewport->GetTerrainBounds().size()) / 20.0f;
	for (int i = 0; i < 18; ++i)
	{
		glm::vec3 currentContentPosition1 = GetTerrainPosition3(currentScreenPositions.first);
		glm::vec3 currentContentPosition2 = GetTerrainPosition3(currentScreenPositions.second);
		glm::vec3 currentDelta = currentContentPosition2 - currentContentPosition1;
		glm::vec3 originalDelta = originalContentPositions.second - originalContentPositions.first;

		float currentAngle = angle(currentDelta.xy());
		float originalAngle = angle(originalDelta.xy());

		Move(contentAnchor, currentScreenCenter);
		Orbit(contentAnchor, originalAngle - currentAngle);

		float k = glm::dot(originalDelta, originalDelta) < glm::dot(currentDelta, currentDelta) ? delta : -delta;
		MoveCamera(_terrainViewport->GetCameraPosition() + k * _terrainViewport->GetCameraDirection());
		delta *= 0.75;
	}
}


void TerrainView::Orbit(glm::vec3 anchor, float angle)
{
	glm::quat rotation = glm::angleAxis(angle, glm::vec3(0, 0, 1));
	glm::vec3 pos = _terrainViewport->GetCameraPosition();
	glm::vec3 value = anchor + rotation * (pos - anchor);
	
	_terrainViewport->SetCameraPosition(value);
	_terrainViewport->SetCameraFacing(_terrainViewport->GetCameraFacing() + angle);

	//MoveCamera(_terrainViewport->GetCameraPosition());
}


void TerrainView::MoveCamera(glm::vec3 position)
{
	bounds2f contentBounds = _terrainViewport->GetTerrainBounds();
	float contentRadius = 0.5f * glm::length(contentBounds.size());

	position.z = (bounds1f(0.05f, 2) * contentRadius).clamp(position.z);

	float h = position.z;
	glm::vec2 p = (glm::vec2)position.xy() - contentBounds.mid();
	p = ::rotate(p, -_terrainViewport->GetCameraFacing());
	float w = contentRadius - p.x;
	float pitch = -atan2f(h, w) - (0.125f - 0.02f) * (float)M_PI;

	if (pitch < -(float)M_PI_2)
		pitch = -(float)M_PI_2;

	float height1 = 0.1f * contentRadius;
	float height2 = 5.0f * contentRadius;
	float t = mix_factor(height1, height2, h);
	t = bounds1f(0, 1).clamp(t);
	pitch = glm::mix(pitch, (float)-M_PI_2, t);

	_terrainViewport->SetCameraPosition(position);
	_terrainViewport->SetCameraTilt(-pitch);
}


void TerrainView::ClampCameraPosition()
{
	glm::vec2 centerScreen = (glm::vec2)_terrainViewport->NormalizedToLocal(glm::vec2(0, 0));
	glm::vec2 contentCamera = GetTerrainPosition2(centerScreen).xy();
	glm::vec2 contentCenter = _terrainViewport->GetTerrainBounds().mid();
	float contentRadius = _heightMap->GetBounds().x().size() / 2;

	glm::vec2 offset = contentCamera - contentCenter;
	float distance = glm::length(offset);
	if (distance > contentRadius)
	{
		glm::vec2 direction = offset / distance;
		_terrainViewport->SetCameraPosition(_terrainViewport->GetCameraPosition() - glm::vec3(direction * (distance - contentRadius), 0));
	}
}


glm::vec2 TerrainView::ContentToScreen(glm::vec3 value) const
{
	glm::mat4 transform = _terrainViewport->GetTransform();
	glm::vec4 p = transform * glm::vec4{value, 1};
	return _terrainViewport->NormalizedToLocal({p.x / p.w, p.y / p.w});
};
