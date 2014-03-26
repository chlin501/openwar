// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include <sstream>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

#include "BattleScript.h"
#include "SamuraiModule.h"
#include "BattleScenario.h"



BattleScript::BattleScript(BattleScenario* scenario) :
_scenario(scenario),
_simulator(scenario->GetSimulator()),
_nextUnitId(1),
_isMaster(true)
{
	_simulator->AddObserver(this);
}


BattleScript::~BattleScript()
{
	_simulator->RemoveObserver(this);
}


void BattleScript::SetIsMaster(bool value)
{
	_isMaster = value;
}



static int random_int(int min, int max)
{
	return min + (int)glm::linearRand<float>(0, max - min);
}


void BattleScript::Execute()
{
	if (_isMaster)
	{
		std::string mapname = (std::ostringstream() << "Map" << random_int(1, 12) << ".tiff").str();
		_scenario->SetSmoothMap(mapname.c_str(), 1024);
	}

	int armytype = random_int(1, 3);
	int count1 = 0;
	int count2 = 0;

	const std::vector<BattleCommander*>& commanders = _scenario->GetCommanders();
	for (int i = 0; i < (int)commanders.size(); ++i)
	{
		switch (commanders[i]->GetTeam())
		{
			case 1: ++count1; break;
			case 2: ++count2; break;
			default: break;
		}
	}

	float xpos1 = -150 * (count1 - 1);
	float xpos2 = -150 * (count2 - 1);

	int part1 = 0;
	int part2 = 0;

	if (count1 > 1)
		part1 = 1;
	if (count2 > 1)
		part2 = 1;

	for (int i = 0; i < (int)commanders.size(); ++i)
	{
		int commanderId = i + 1;
		int team = commanders[i]->GetTeam();
		int position = _scenario->GetTeamPosition(team);
		BattleCommanderType type = commanders[i]->GetType();

		if (type != BattleCommanderType::None)
		{
			int& part = team == 1 ? part1 : part2;

			std::function<glm::vec2(float, float)> pos = position == 1
				? std::function<glm::vec2(float, float)>([xpos1](float x, float y) { return glm::vec2(x + xpos1, y); })
				: std::function<glm::vec2(float, float)>([xpos2](float x, float y) { return glm::vec2(x + xpos2, 1024 - y); });

			float bearing = team == 1 ? 0 : 180;

			if (armytype == 1)
			{
				if (part == 0 || part == 1)
				{
					NewUnit(commanderId, "CAV-BOW",  40, pos(354, 244), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(371, 277), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(392, 311), bearing);
					NewUnit(commanderId, "ASH-BOW",  80, pos(400, 345), bearing);
					NewUnit(commanderId, "GEN-KATA", 40, pos(429, 213), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(452, 255), bearing);
				}
				if (part == 0 || part == 2)
				{
					NewUnit(commanderId, "SAM-NAGI", 80, pos(461, 302), bearing);
					NewUnit(commanderId, "ASH-BOW",  80, pos(468, 339), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(531, 294), bearing);
					NewUnit(commanderId, "ASH-BOW",  80, pos(533, 332), bearing);
					NewUnit(commanderId, "CAV-YARI", 40, pos(537, 231), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(549, 262), bearing);
					NewUnit(commanderId, "CAV-KATA", 40, pos(645, 204), bearing);
				}
			}
			else if (armytype == 2)
			{
				if (part == 0 || part == 1)
				{
					NewUnit(commanderId, "CAV-KATA", 40, pos(225, 340), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(294, 270), bearing);
					NewUnit(commanderId, "SAM-BOW",  80, pos(332, 307), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(342, 337), bearing);
					NewUnit(commanderId, "SAM-NAGI", 80, pos(362, 271), bearing);
					NewUnit(commanderId, "ASH-ARQ",  80, pos(403, 368), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(465, 254), bearing);
				}
				if (part == 0 || part == 2)
				{
					NewUnit(commanderId, "GEN-KATA", 40, pos(465, 314), bearing);
					NewUnit(commanderId, "ASH-ARQ",  80, pos(472, 366), bearing);
					NewUnit(commanderId, "ASH-ARQ",  80, pos(529, 367), bearing);
					NewUnit(commanderId, "SAM-NAGI", 80, pos(551, 271), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(580, 342), bearing);
					NewUnit(commanderId, "SAM-BOW",  80, pos(583, 312), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(626, 276), bearing);
					NewUnit(commanderId, "CAV-KATA", 40, pos(683, 346), bearing);
				}
			}
			else
			{
				if (part == 0 || part == 1)
				{
					NewUnit(commanderId, "SAM-NAGI", 80, pos(383, 324), bearing);
					NewUnit(commanderId, "CAV-KATA", 40, pos(359, 286), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(443, 348), bearing);
					NewUnit(commanderId, "ASH-ARQ",  80, pos(448, 419), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(456, 386), bearing);
					NewUnit(commanderId, "ASH-BOW",  80, pos(503, 359), bearing);
					NewUnit(commanderId, "GEN-KATA", 40, pos(507, 315), bearing);
				}
				else if (part == 0 || part == 2)
				{
					NewUnit(commanderId, "SAM-NAGI", 80, pos(508, 269), bearing);
					NewUnit(commanderId, "ASH-YARI", 80, pos(552, 387), bearing);
					NewUnit(commanderId, "ASH-ARQ",  80, pos(558, 414), bearing);
					NewUnit(commanderId, "SAM-KATA", 80, pos(562, 350), bearing);
					NewUnit(commanderId, "CAV-BOW",  40, pos(679, 308), bearing);
					NewUnit(commanderId, "CAV-YARI", 40, pos(601, 284), bearing);
					NewUnit(commanderId, "SAM-NAGI", 80, pos(616, 333), bearing);
				}
			}

			++part;
		}

		if (team == 1)
			xpos1 += 300;
		else
			xpos2 += 300;
	}
}


void BattleScript::Tick(double secondsSinceLastUpdate)
{
}


/* BattleObserver */


void BattleScript::OnSetGroundMap(GroundMap* groundMap)
{

}


void BattleScript::OnAddUnit(Unit* unit)
{
	if (_unitId[unit] == 0)
	{
		int unitId = _nextUnitId++;
		_units[unitId] = unit;
		_unitId[unit] = unitId;
	}
}


void BattleScript::OnRemoveUnit(Unit* unit)
{
	int unitId = _unitId[unit];
	_units.erase(unitId);
	_unitId.erase(unit);
}


void BattleScript::OnCommand(Unit* unit, float timer)
{
}


void BattleScript::OnShooting(const Shooting& shooting, float timer)
{
}


void BattleScript::OnRelease(const Shooting& shooting)
{
}


void BattleScript::OnCasualty(const Fighter& fighter)
{
}


void BattleScript::OnRouting(Unit* unit)
{
}


/***/


int BattleScript::NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing)
{
	UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

	BattleCommander* commander = _scenario->GetCommanders()[commanderId - 1];

	Unit* unit = _simulator->AddUnit(commander, unitClass, strength, unitStats, position);
	unit->command.bearing = glm::radians(90 - bearing);

	int unitId = _nextUnitId++;
	_units[unitId] = unit;
	_unitId[unit] = unitId;

	return unitId;
}
