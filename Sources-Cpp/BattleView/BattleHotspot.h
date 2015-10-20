#ifndef BattleHotspot_H
#define BattleHotspot_H

#include "Surface/Hotspot.h"
#include "BattleGesture.h"


class BattleHotspot : public Hotspot
{
	BattleGesture _gesture;
	BattleView* _battleView;

public:
	BattleHotspot(BattleView* battleView);
	virtual ~BattleHotspot();

	Gesture* GetGesture() override;

	BattleView* GetBattleView() const;
};


#endif
