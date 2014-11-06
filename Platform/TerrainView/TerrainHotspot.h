#ifndef TerrainHotspot_H
#define TerrainHotspot_H

#include "Hotspot.h"
#include "TerrainGesture.h"


class TerrainHotspot : public Hotspot<TerrainHotspot, TerrainGesture>
{
	TerrainView* _terrainView;

public:
	TerrainHotspot(TerrainView* terrainView);
	virtual ~TerrainHotspot();

	TerrainView* GetTerrainView() const;
};


#endif
