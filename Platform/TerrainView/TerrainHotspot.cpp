#include "TerrainHotspot.h"
#import "TerrainView.h"


TerrainHotspot::TerrainHotspot(TerrainView* terrainView) :
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
