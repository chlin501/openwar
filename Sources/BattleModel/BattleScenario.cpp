#include <cstdlib>
#include "../Library/resource.h"
#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "SamuraiModule.h"
#include "BattleScript.h"


BattleScenario::BattleScenario(const char* name) :
_name(name),
_simulator(nullptr),
_script(nullptr)
{
	_simulator = new BattleSimulator();
	_script = new BattleScript(this);
}


BattleScenario::~BattleScenario()
{
	for (BattleCommander* commander : _commanders)
		delete commander;

	GroundMap* groundMap = _simulator->GetGroundMap();
	delete _simulator;
	delete groundMap;
}


void BattleScenario::AddCommander(const char* id, BattleCommanderType type, const char* configuration)
{
	BattleCommander* commander = new BattleCommander(id, type, configuration);
	_commanders.push_back(commander);
}


void BattleScenario::Start(bool master)
{
	if (!_name.empty())
	{
		std::string directory = resource("Maps/").path();
		std::string package_path = directory + "/?.lua";

		_script->SetGlobalNumber("openwar_seed", std::rand());
		_script->SetGlobalString("openwar_script_directory", directory.c_str());
		_script->AddStandardPath();
		_script->AddPackagePath(package_path.c_str());

		resource source(_name.c_str());
		if (source.load())
		{
			_script->Execute((const char*)source.data(), source.size());
		}
	}
}


void BattleScenario::Tick(double secondsSinceLastTick)
{
	_script->Tick(secondsSinceLastTick);
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
