#include "BattleHotspot.h"
#include "BattleView.h"


BattleHotspot::BattleHotspot(BattleView* battleView) :
	_gesture(this),
	_battleView(battleView)
{
}


Gesture* BattleHotspot::GetGesture()
{
	return &_gesture;
}


BattleHotspot::~BattleHotspot()
{
}


BattleView* BattleHotspot::GetBattleView() const
{
	return _battleView;
}
