#ifndef BattleMap_H
#define BattleMap_H

#include <vector>

class HeightMap;
class GroundMap;
class BattleMap;


class BattleMapObserver
{
public:
	virtual ~BattleMapObserver();
	virtual void OnBattleMapChanged(const BattleMap* battleMap) const = 0;
};


class BattleMap
{
	std::vector<BattleMapObserver*> _observers{};

public:
	virtual ~BattleMap();

	void AddObserver(BattleMapObserver* observer);
	void RemoveObserver(BattleMapObserver* observer);

	virtual const HeightMap* GetHeightMap() const = 0;
	virtual const GroundMap* GetGroundMap() const = 0;

protected:
	void NotifyBattleMapChanged();
};


#endif
