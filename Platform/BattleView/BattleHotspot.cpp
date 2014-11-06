#include "BattleHotspot.h"
#include "BattleView.h"


BattleHotspot::BattleHotspot(BattleView* battleView) :
	_battleView(battleView)
{
}


BattleHotspot::~BattleHotspot()
{
}


BattleView* BattleHotspot::GetBattleView() const
{
	return _battleView;
}
