#ifndef BattleMap_H
#define BattleMap_H

#include <vector>

class BattleMap;
class GroundMap;
class HeightMap;
class MapEditor;


class BattleMapObserver
{
	friend class BattleMap;

public:
	virtual ~BattleMapObserver();

private:
	virtual void OnBattleMapChanged(const BattleMap* battleMap) = 0;
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
	virtual MapEditor* GetMapEditor() const = 0;


protected:
	void NotifyBattleMapChanged();
};



class BasicBattleMap : public BattleMap
{
	const HeightMap* _heightMap;
	const GroundMap* _groundMap;
public:
	BasicBattleMap(const HeightMap* heightMap, const GroundMap* groundMap);

	const HeightMap* GetHeightMap() const override;
	const GroundMap* GetGroundMap() const override;
	MapEditor* GetMapEditor() const override;
};

#endif
