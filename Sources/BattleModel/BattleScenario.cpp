#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "SamuraiModule.h"


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
	BattleCommander* commander = new BattleCommander(id, type, configuration);
	_commanders.push_back(commander);
}


void BattleScenario::AddUnits()
{
	int player = 1;
	int team = 1;

	for (BattleCommander* commander : _commanders)
	{
		if (commander->GetType() != BattleCommanderType::None)
		{
			float xpos = 500 + 40 * player;
			float ypos = team == 1 ? 512 + 40 : 512 - 40;
			glm::vec2 position = glm::vec2(xpos, ypos);
			const char* unitClass = "SAM-KATA";
			float bearing = 0;

			UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

			Unit* unit = _simulator->AddUnit(player, team, unitClass, 60, unitStats, position);
			unit->command.bearing = glm::radians(90 - bearing);

			++player;
			team = team == 1 ? 2 : 1;
		}
	}
}
