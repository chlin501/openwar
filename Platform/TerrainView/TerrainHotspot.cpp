#include "TerrainHotspot.h"
#import "TerrainView.h"


TerrainHotspot::TerrainHotspot(TerrainView* terrainView) :
	_gesture(this),
	_terrainView(terrainView)
{
}


Gesture* TerrainHotspot::GetGesture()
{
	return &_gesture;
}


TerrainHotspot::~TerrainHotspot()
{
}


TerrainView* TerrainHotspot::GetTerrainView() const
{
	return _terrainView;
}
