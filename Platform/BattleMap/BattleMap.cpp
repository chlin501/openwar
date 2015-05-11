#include "BattleMap.h"


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
