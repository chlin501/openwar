#include "BattleScenario.h"
#include "BattleSimulator.h"


BattleScenario::BattleScenario(BattleSimulator* battleSimulator) :
	_battleSimulator(battleSimulator)
{

}


BattleScenario::~BattleScenario()
{
}


void BattleScenario::Tick(float secondsSinceLastTick)
{
}
