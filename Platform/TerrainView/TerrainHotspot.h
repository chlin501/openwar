#ifndef TerrainHotspot_H
#define TerrainHotspot_H

#include "Surface/Hotspot.h"
#include "TerrainGesture.h"


class TerrainHotspot : public Hotspot
{
	TerrainGesture _gesture;
	TerrainView* _terrainView;

public:
	TerrainHotspot(TerrainView* terrainView);
	virtual ~TerrainHotspot();

	Gesture* GetGesture() override;

	TerrainView* GetTerrainView() const;
};


#endif
