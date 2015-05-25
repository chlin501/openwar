#include "TerrainHotspot.h"
#include "TerrainView.h"
#include "TerrainViewport.h"
#include "Surface/Touch.h"

#include <glm/gtc/constants.hpp>


TerrainHotspot::TerrainHotspot(TerrainView* terrainView) :
	_gesture(this),
	_terrainView(terrainView)
{
}


TerrainHotspot::~TerrainHotspot()
{
}


TerrainView* TerrainHotspot::GetTerrainView() const
{
	return _terrainView;
}


Gesture* TerrainHotspot::GetGesture()
{
	return &_gesture;
}


bool TerrainHotspot::TryCaptureTouch(Touch* touch)
{
	if (touch->IsCaptured()
		|| GetCapturedTouches().size() == 2
		|| !Hotspot::TryCaptureTouch(touch))
	{
		return false;
	}

	glm::vec3& contentPosition = GetCapturedTouches().size() == 1 ? _contentPosition1 : _contentPosition2;
	contentPosition = _terrainView->GetTerrainPosition3(touch->GetCurrentPosition());

	return true;
}


void TerrainHotspot::ReleaseTouch(Touch* touch)
{
	if (touch == GetCapturedTouches()[0])
		_contentPosition1 = _contentPosition2;

	Hotspot::ReleaseTouch(touch);
}


TerrainHotspotMode TerrainHotspot::GetTerrainHotspotMode() const
{
	switch (GetCapturedTouches().size())
	{
		case 1: return TerrainHotspotMode::MoveAndOrbit;
		case 2: return TerrainHotspotMode::Zoom;
		default: return TerrainHotspotMode::None;
	}
}



glm::vec3 TerrainHotspot::GetContentPosition1() const
{
	if (GetCapturedTouches().size() >= 1)
		return _contentPosition1;

	return _terrainView->GetTerrainPosition3(GetCurrentScreenPosition1());
}


glm::vec3 TerrainHotspot::GetContentPosition2() const
{
	if (GetCapturedTouches().size() >= 2)
		return _contentPosition2;
	return GetContentPosition2x();
};


glm::vec3 TerrainHotspot::GetContentPosition2x() const
{
	glm::vec3 p = GetContentPosition1();
	glm::vec3 c = _terrainView->GetTerrainPosition3(_terrainView->GetTerrainViewport()->NormalizedToLocal(glm::vec2{}));
	return glm::vec3{2.0f * c.xy() - p.xy(), p.z};
}


std::pair<glm::vec3, glm::vec3> TerrainHotspot::GetContentPositions() const
{
	return std::make_pair(GetContentPosition1(), GetContentPosition2());
}


glm::vec2 TerrainHotspot::GetCurrentScreenPosition1() const
{
	if (GetCapturedTouches().size() >= 1)
		return GetCapturedTouches().front()->GetCurrentPosition();

	return _terrainView->GetTerrainViewport()->NormalizedToLocal(glm::vec2{});
}


glm::vec2 TerrainHotspot::GetCurrentScreenPosition2() const
{
	if (GetCapturedTouches().size() >= 2)
		return GetCapturedTouches().back()->GetCurrentPosition();
	return GetCurrentScreenPosition2x();
}


glm::vec2 TerrainHotspot::GetCurrentScreenPosition2x() const
{
	glm::vec3 p = _terrainView->GetTerrainPosition3(GetCurrentScreenPosition1());
	glm::vec3 c = _terrainView->GetTerrainPosition3(_terrainView->GetTerrainViewport()->NormalizedToLocal(glm::vec2{}));
	return _terrainView->ContentToScreen(glm::vec3(2.0f * c.xy() - p.xy(), p.z));
}


std::pair<glm::vec2, glm::vec2> TerrainHotspot::GetCurrentScreenPositions() const
{
	return std::make_pair(GetCurrentScreenPosition1(), GetCurrentScreenPosition2());
}


glm::vec3 TerrainHotspot::GetOrbitAnchor() const
{
	return 0.5f * (GetContentPosition1() + GetContentPosition2x());
}


float TerrainHotspot::GetOrbitFactor() const
{
	glm::vec2 pos = GetCapturedTouches().size() == 1
		? GetCurrentScreenPosition1()
		: 0.5f * (GetCurrentScreenPosition1() + GetCurrentScreenPosition2());

	float value = glm::length(_terrainView->GetTerrainViewport()->LocalToNormalized(pos));
	return bounds1f{0, 1}.clamp(bounds1f{0.33f, 0.66f}.unmix(value * value));
};


float TerrainHotspot::GetOrbitAngle() const
{
	glm::vec3 currentPosition1 = _terrainView->GetTerrainPosition3(GetCurrentScreenPosition1());
	glm::vec3 currentPosition2 = _terrainView->GetTerrainPosition2(GetCurrentScreenPosition2x(), currentPosition1.z);

	float currentAngle = angle(currentPosition1.xy() - currentPosition2.xy());
	float contentAngle = angle(GetContentPosition1().xy() - GetContentPosition2x().xy());

	return contentAngle - currentAngle;
}


void TerrainHotspot::RenderMouseHint(VertexShape_3f& vertices)
{
	if (renderMouseHits)
	{
		glm::vec3 p1 = GetContentPosition1();
		glm::vec3 p2 = GetContentPosition2();
		RenderCross(vertices, p1, 16);
		RenderCross(vertices, p2, 16);
		vertices.AddVertex({p1});
		vertices.AddVertex({p2});

		glm::vec3 c1 = _terrainView->GetTerrainPosition3(GetCurrentScreenPosition1());
		glm::vec3 c2 = _terrainView->GetTerrainPosition3(GetCurrentScreenPosition2());
		RenderCross(vertices, c1, 8);
		RenderCross(vertices, c2, 8);
		vertices.AddVertex({c1});
		vertices.AddVertex({c2});

		glm::vec3 a = GetOrbitAnchor();
		float d = glm::distance(p1, a);
		RenderCircle(vertices, a, d);
	}
};


void TerrainHotspot::RenderCross(VertexShape_3f& vertices, glm::vec3 p, float d)
{
	vertices.AddVertex({p + glm::vec3(0, 0, -d)});
	vertices.AddVertex({p + glm::vec3(0, 0, d)});

	vertices.AddVertex({p + glm::vec3(-d, 0, -d)});
	vertices.AddVertex({p + glm::vec3(d, 0, d)});

	vertices.AddVertex({p + glm::vec3(d, 0, -d)});
	vertices.AddVertex({p + glm::vec3(-d, 0, d)});

	vertices.AddVertex({p + glm::vec3(0, d, -d)});
	vertices.AddVertex({p + glm::vec3(0, -d, d)});

	vertices.AddVertex({p + glm::vec3(0, -d, -d)});
	vertices.AddVertex({p + glm::vec3(0, d, d)});
}


void TerrainHotspot::RenderCircle(VertexShape_3f& vertices, glm::vec3 p, float d)
{
	const float n = 32;
	const float two_pi_over_n = 2 * glm::pi<float>() / n;
	for (float i = 0; i < n; ++i)
	{
		float a1 = two_pi_over_n * i;
		float a2 = two_pi_over_n * (i + 1);

		glm::vec2 d1 = glm::vec2{glm::cos(a1), glm::sin(a1)};
		glm::vec2 d2 = glm::vec2{glm::cos(a2), glm::sin(a2)};

		vertices.AddVertex({p + d * glm::vec3{d1, 0}});
		vertices.AddVertex({p + d * glm::vec3{d2, 0}});
	};
}
