// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BattleModel/HeightMap.h"
#include "TerrainView.h"


TerrainView::TerrainView() : Content(),
_cameraTilt((float)M_PI_4),
_cameraFacing(0),
_mouseHintVisible(false),
_heightMap(nullptr)
{
}


TerrainView::~TerrainView()
{

}


void TerrainView::ShowMouseHint(glm::vec2 position)
{
	_mouseHintPosition = position;
	_mouseHintVisible = true;
}


void TerrainView::HideMouseHint()
{
	_mouseHintVisible = false;
}


void TerrainView::RenderMouseHint(VertexShape_3f* vertices)
{
	if (_mouseHintVisible)
	{
		glm::vec3 p = GetTerrainPosition3(_mouseHintPosition);
		if (p.z < 0)
			p.z = 0;
		float d = 5;

		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, 0, -d)));
		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, 0, d)));

		vertices->AddVertex(Vertex_3f(p + glm::vec3(-d, 0, -d)));
		vertices->AddVertex(Vertex_3f(p + glm::vec3(d, 0, d)));

		vertices->AddVertex(Vertex_3f(p + glm::vec3(d, 0, -d)));
		vertices->AddVertex(Vertex_3f(p + glm::vec3(-d, 0, d)));

		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, d, -d)));
		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, -d, d)));

		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, -d, -d)));
		vertices->AddVertex(Vertex_3f(p + glm::vec3(0, d, d)));
	}
}


void TerrainView::SetHeightMap(HeightMap* heightMap)
{
	_heightMap = heightMap;

	_contentBounds = heightMap->GetBounds();

	/*
	glm::vec2 center = _contentBounds.center();
	glm::vec2 size = _contentBounds.size();

	_cameraPosition = glm::vec3(center - 0.2f * size, 0.2f * size.y);
	_cameraTilt = (float)M_PI_4;
	_cameraFacing = 0;
	MoveCamera(_cameraPosition);*/
}


float TerrainView::GetContentRadius() const
{
	return _contentBounds.height() / 2;
}


glm::vec2 TerrainView::GetScreenTop() const
{
	glm::vec2 center = _contentBounds.center();
	float angle = _cameraFacing;
	if (GetFlip())
		angle += (float)M_PI;
	float radius = GetContentRadius();

	return ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle), 0));
}


glm::vec2 TerrainView::GetScreenLeft() const
{
	glm::vec2 center = _contentBounds.center();
	float angle = _cameraFacing;
	if (GetFlip())
		angle += (float)M_PI;
	float radius = GetContentRadius();

	glm::vec2 result(GetFrame().max.x, 0);

	int n = 20;
	for (int i = 0; i < n; ++i)
	{
		float a = i * (float)M_PI / n;
		glm::vec2 p = ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle + a), 0));
		if (p.x < result.x)
			result = p;
	}

	return result;
}


glm::vec2 TerrainView::GetScreenBottom() const
{
	glm::vec2 center = _contentBounds.center();
	float angle = _cameraFacing + (float)M_PI;
	if (GetFlip())
		angle += (float)M_PI;
	float radius = GetContentRadius();

	glm::vec2 result = ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle), 0));

	if (result.y > GetFrame().max.y)
		result.y = GetFrame().min.y;

	return result;
}


glm::vec2 TerrainView::GetScreenRight() const
{
	glm::vec2 center = _contentBounds.center();
	float angle = _cameraFacing;
	if (GetFlip())
		angle += (float)M_PI;
	float radius = GetContentRadius();

	glm::vec2 result(GetFrame().min.x, 0);

	int n = 20;
	for (int i = 4; i < n - 4; ++i)
	{
		float a = i * (float)M_PI / n;
		glm::vec2 p = ContentToScreen(glm::vec3(center + radius * vector2_from_angle(angle - a), 0));
		if (p.x > result.x)
			result = p;
	}

	return result;
}


glm::vec3 TerrainView::GetCameraDirection() const
{
	glm::quat q = glm::angleAxis(_cameraFacing, glm::vec3(0, 0, 1));
	glm::vec3 v(cosf(_cameraTilt), 0, -sinf(_cameraTilt));
	return q * v;
}


glm::vec3 TerrainView::GetCameraUpVector() const
{
	glm::quat q = glm::angleAxis(_cameraFacing, glm::vec3(0, 0, 1));
	glm::vec3 v(sinf(_cameraTilt), 0, cosf(_cameraTilt));
	return q * v;
}


glm::mat4x4 TerrainView::GetProjectionMatrix() const
{
	float r = 2 * glm::length(_contentBounds.size());

	glm::vec2 frameSize = GetFrame().size();
	float frameAspect = frameSize.y / frameSize.x;

	return glm::perspective(45.0f, 1.0f / frameAspect, 0.01f * r, r);
}


glm::mat4x4 TerrainView::GetViewMatrix() const
{
	glm::mat4x4 result = glm::lookAt(_cameraPosition, _cameraPosition + GetCameraDirection(), GetCameraUpVector());
	if (GetFlip())
		result = glm::scale(glm::mat4x4(), glm::vec3(-1.0f, -1.0f, 1.0f)) * result;
	return result;
}


ray TerrainView::GetCameraRay(glm::vec2 screenPosition) const
{
	glm::vec2 viewPosition = SurfaceToContent(screenPosition);
	glm::mat4x4 inverse = glm::inverse(GetTerrainTransform());
	glm::vec4 p1 = inverse * glm::vec4(viewPosition, 0, 1.0f);
	glm::vec4 p2 = inverse * glm::vec4(viewPosition, 0.5f, 1.0f);

	glm::vec3 q1 = (glm::vec3)p1.xyz() / p1.w;
	glm::vec3 q2 = (glm::vec3)p2.xyz() / p2.w;

	glm::vec3 direction =  glm::normalize(q2 - q1);
	glm::vec3 origin = q1 - 200.0f * direction;
	return ray(origin, direction);
}


glm::vec3 TerrainView::GetTerrainPosition2(glm::vec2 screenPosition) const
{
	ray r = GetCameraRay(screenPosition);
	const float* d = intersect(r, plane(glm::vec3(0, 0, 1), 0));
	return r.point(d != nullptr ? *d : 0);
}


glm::vec3 TerrainView::GetTerrainPosition3(glm::vec2 screenPosition) const
{
	if (_heightMap == nullptr)
		return glm::vec3();

	ray r = GetCameraRay(screenPosition);
	const float* d = _heightMap->Intersect(r);
	return r.point(d != nullptr ? *d : 0);
}


void TerrainView::Move(glm::vec3 originalContentPosition, glm::vec2 currentScreenPosition)
{
	ray ray1 = GetCameraRay(currentScreenPosition);
	ray ray2 = ray(originalContentPosition, -ray1.direction);

	plane cameraPlane(glm::vec3(0, 0, 1), _cameraPosition);
	const float* d = intersect(ray2, cameraPlane);
	if (d != nullptr)
	{
		MoveCamera(ray2.point(*d));
	}
}


void TerrainView::Zoom(glm::vec3 originalContentPosition1, glm::vec3 originalContentPosition2, glm::vec2 currentScreenPosition1, glm::vec2 currentScreenPosition2, float orbitFactor)
{
	glm::vec3 originalContentCenter = (originalContentPosition1 + originalContentPosition2) / 2.0f;
	glm::vec2 currentScreenCenter = (currentScreenPosition1 + currentScreenPosition2) / 2.0f;

	float delta = (1 - orbitFactor) * glm::length(GetContentBounds().size()) / 20.0f;
	for (int i = 0; i < 18; ++i)
	{
		glm::vec3 currentContentPosition1 = GetTerrainPosition3(currentScreenPosition1);
		glm::vec3 currentContentPosition2 = GetTerrainPosition3(currentScreenPosition2);
		glm::vec3 currentDelta = currentContentPosition2 - currentContentPosition1;
		glm::vec3 originalDelta = originalContentPosition2 - originalContentPosition1;

		float currentAngle = angle(currentDelta.xy());
		float originalAngle = angle(originalDelta.xy());

		Move(originalContentCenter, currentScreenCenter);
		Orbit(originalContentCenter.xy(), originalAngle - currentAngle);

		float k = glm::dot(originalDelta, originalDelta) < glm::dot(currentDelta, currentDelta) ? delta : -delta;
		MoveCamera(_cameraPosition + k * GetCameraDirection());
		delta *= 0.75;
	}
}


void TerrainView::Orbit(glm::vec2 originalContentPosition, float angle)
{
	glm::quat rotation = glm::angleAxis(angle, glm::vec3(0, 0, 1));

	glm::vec3 center(originalContentPosition.x, originalContentPosition.y, 0);
	glm::vec3 delta = _cameraPosition - center;

	_cameraPosition = center + rotation * delta;
	_cameraFacing += angle;

	MoveCamera(_cameraPosition);
}


void TerrainView::MoveCamera(glm::vec3 position)
{
	bounds2f contentBounds = GetContentBounds();
	float contentRadius = 0.5f * glm::length(contentBounds.size());

	position.z = (bounds1f(0.05f, 2) * contentRadius).clamp(position.z);

	float h = position.z;
	glm::vec2 p = (glm::vec2)position.xy() - contentBounds.center();
	p = ::rotate(p, -_cameraFacing);
	float w = contentRadius - p.x;
	float pitch = -atan2f(h, w) - (0.125f - 0.02f) * (float)M_PI;

	if (pitch < -(float)M_PI_2)
		pitch = -(float)M_PI_2;

	float height1 = 0.1f * contentRadius;
	float height2 = 5.0f * contentRadius;
	float t = mix_factor(height1, height2, h);
	t = bounds1f(0, 1).clamp(t);
	pitch = glm::mix(pitch, (float)-M_PI_2, t);

	_cameraPosition = position;
	_cameraTilt = -pitch;
}


void TerrainView::ClampCameraPosition()
{
	glm::vec2 centerScreen = GetFrame().center();
	glm::vec2 contentCamera = GetTerrainPosition2(centerScreen).xy();
	glm::vec2 contentCenter = GetContentBounds().center();
	float contentRadius = _heightMap->GetBounds().width() / 2;

	glm::vec2 offset = contentCamera - contentCenter;
	float distance = glm::length(offset);
	if (distance > contentRadius)
	{
		glm::vec2 direction = offset / distance;
		_cameraPosition -= glm::vec3(direction * (distance - contentRadius), 0);
	}
}


static glm::vec3 transform_d(const glm::mat4x4& m, glm::vec3 v)
{
	const glm::mat4x4::value_type* mm = glm::value_ptr(m);
	double x = v.x;
	double y = v.y;
	double z = v.z;
	double w = 1;
	double x1 = mm[0] * x + mm[4] * y + mm[8] * z + mm[12] * w;
	double y1 = mm[1] * x + mm[5] * y + mm[9] * z + mm[13] * w;
	double z1 = mm[2] * x + mm[6] * y + mm[10] * z + mm[14] * w;
	double w1 = mm[3] * x + mm[7] * y + mm[11] * z + mm[15] * w;

	return glm::vec3((float)(x1 / w1), (float)(y1 / w1), (float)(z1 / w1));
}


glm::vec3 TerrainView::ScreenToContent(glm::vec2 value) const
{
	glm::mat4x4 transform = glm::inverse(GetTerrainTransform());
	glm::vec2 p = SurfaceToContent(value);
	return transform_d(transform, glm::vec3(p, 0));
}


glm::vec2 TerrainView::ContentToScreen(glm::vec3 value) const
{
	glm::mat4x4 transform = GetTerrainTransform();
	glm::vec3 v = transform_d(transform, value);
	return ContentToSurface(v.xy());
}
