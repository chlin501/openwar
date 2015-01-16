#ifndef TerrainHotspot_H
#define TerrainHotspot_H

#include "Surface/Hotspot.h"
#include "Shapes/VertexShape.h"
#include "TerrainGesture.h"


enum class TerrainHotspotMode { None, Move, Zoom };


class TerrainHotspot : public Hotspot
{
	TerrainGesture _gesture;
	TerrainView* _terrainView;
	glm::vec3 _contentPosition1;
	glm::vec3 _contentPosition2;
	bool renderMouseHits{};

public:
	TerrainHotspot(TerrainView* terrainView);
	virtual ~TerrainHotspot();

	TerrainView* GetTerrainView() const;

	Gesture* GetGesture() override;

	bool TryCaptureTouch(Touch* touch) override;
	void ReleaseTouch(Touch* touch) override;

	TerrainHotspotMode GetTerrainHotspotMode() const;

	glm::vec3 GetContentPosition1() const;
	glm::vec3 GetContentPosition2() const;
	glm::vec3 GetContentPosition2x() const;
	std::pair<glm::vec3, glm::vec3> GetContentPositions() const;

	glm::vec2 GetCurrentScreenPosition1() const;
	glm::vec2 GetCurrentScreenPosition2() const;
	glm::vec2 GetCurrentScreenPosition2x() const;
	std::pair<glm::vec2, glm::vec2> GetCurrentScreenPositions() const;

	glm::vec3 GetOrbitAnchor() const;
	float GetOrbitFactor() const;
	float GetOrbitAngle() const;

	void RenderMouseHint(VertexShape_3f& vertices);

private:
	static void RenderCross(VertexShape_3f& vertices, glm::vec3 p, float d);
	static void RenderCircle(VertexShape_3f& vertices, glm::vec3 p, float d);
};


#endif
