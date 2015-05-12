#include "BattleMap.h"
#import "HeightMap.h"
#import "GroundMap.h"


BattleMapObserver::~BattleMapObserver()
{
}


BattleMap::~BattleMap()
{
}


void BattleMap::AddObserver(BattleMapObserver* observer)
{
	_observers.push_back(observer);
}


void BattleMap::RemoveObserver(BattleMapObserver* observer)
{
	_observers.erase(
		std::remove(_observers.begin(), _observers.end(), observer),
		_observers.end());
}


void BattleMap::NotifyBattleMapChanged()
{
	for (BattleMapObserver* observer : _observers)
		observer->OnBattleMapChanged(this);
}


BasicBattleMap::BasicBattleMap(const HeightMap* heightMap, const GroundMap* groundMap) :
	_heightMap(heightMap),
	_groundMap(groundMap)
{
}


const HeightMap* BasicBattleMap::GetHeightMap() const
{
	return _heightMap;
}


const GroundMap* BasicBattleMap::GetGroundMap() const
{
	return _groundMap;
}
