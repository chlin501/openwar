#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"


BattleScenario::BattleScenario() :
_simulator(nullptr)
{
	_simulator = new BattleSimulator();
}


BattleScenario::~BattleScenario()
{
	GroundMap* groundMap = _simulator->GetGroundMap();
	delete _simulator;
	delete groundMap;
}


void BattleScenario::AddCommander(const char* id, BattleCommanderType type, const char* configuration)
{
	BattleCommander* commander = new BattleCommander(id, type);
	_commanders.push_back(commander);
}
