#ifndef BattleHotspot_H
#define BattleHotspot_H

#include "Hotspot.h"
#include "BattleGesture.h"


class BattleHotspot : public Hotspot
{
	BattleGesture _gesture;
	BattleView* _battleView;

public:
	BattleHotspot(BattleView* battleView);
	virtual ~BattleHotspot();

	virtual Gesture* GetGesture();

	BattleView* GetBattleView() const;
};


#endif
