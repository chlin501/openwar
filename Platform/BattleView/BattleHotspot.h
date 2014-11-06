#ifndef BattleHotspot_H
#define BattleHotspot_H

#include "Hotspot.h"
#include "BattleGesture.h"


class BattleHotspot : public Hotspot<BattleHotspot, BattleGesture>
{
	BattleView* _battleView;

public:
	BattleHotspot(BattleView* battleView);
	virtual ~BattleHotspot();

	BattleView* GetBattleView() const;
};


#endif
