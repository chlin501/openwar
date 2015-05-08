// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <cstring>
#include <sstream>
#include <functional>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

#include "BattleScript.h"
#include "SamuraiModule.h"
#include "BattleScenario.h"



BattleScript::BattleScript(BattleScenario* scenario) :
	_scenario{scenario},
	_simulator{scenario->GetSimulator()}
{
}


BattleScript::~BattleScript()
{
}


static int random_int(int min, int max)
{
	return min + (int)glm::linearRand<float>(0, max - min);
}


void BattleScript::Execute()
{
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

	int part1 = 0;
	int part2 = 0;

	if (count1 > 1)
		part1 = 1;
	if (count2 > 1)
		part2 = 1;

	for (int i = 0; i < (int)commanders.size(); ++i)
	{
		int team = commanders[i]->GetTeam();
		int commanderId = i + 1;
		int position = _scenario->GetTeamPosition(team);
		float bearing = position == 1 ? 0 : 180;

		int* part = team == 1 ? &part1 : &part2;

		std::function<glm::vec2(int, int)> pos = position == 1
			? std::function<glm::vec2(int, int)>([](int x, int y) { return glm::vec2(512 + x, 256 - 32 + y); })
			: std::function<glm::vec2(int, int)>([](int x, int y) { return glm::vec2(512 - x, 768 + 32 - y); });


		if (armytype == 1)
		{
			if (*part == 0 || *part == 1)
			{
				NewUnit(commanderId, "CAV-BOW",  40, pos(-117, -34), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(-100, -1), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(-79, 33), bearing);
				NewUnit(commanderId, "ASH-BOW",  80, pos(-71, 67), bearing);
				NewUnit(commanderId, "GEN-KATA", 40, pos(-42, -65), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(-19, -23), bearing);
			}
			if (*part == 0 || *part == 2)
			{
				NewUnit(commanderId, "SAM-NAGI", 80, pos(-10, 24), bearing);
				NewUnit(commanderId, "ASH-BOW",  80, pos(-3, 61), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(60, 16), bearing);
				NewUnit(commanderId, "ASH-BOW",  80, pos(62, 54), bearing);
				NewUnit(commanderId, "CAV-YARI", 40, pos(66, -47), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(78, -16), bearing);
				NewUnit(commanderId, "CAV-KATA", 40, pos(174, -74), bearing);
			}
		}
		else if (armytype == 2)
		{
			if (*part == 0 || *part == 1)
			{
				NewUnit(commanderId, "CAV-KATA", 40, pos(-236, 24), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(-167, -46), bearing);
				NewUnit(commanderId, "SAM-BOW",  80, pos(-129, -9), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(-119, 21), bearing);
				NewUnit(commanderId, "SAM-NAGI", 80, pos(-99, -45), bearing);
				NewUnit(commanderId, "ASH-ARQ",  80, pos(-58, 52), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(4, -62), bearing);
			}
			if (*part == 0 || *part == 2)
			{
				NewUnit(commanderId, "GEN-KATA", 40, pos(4, -2), bearing);
				NewUnit(commanderId, "ASH-ARQ",  80, pos(11, 50), bearing);
				NewUnit(commanderId, "ASH-ARQ",  80, pos(68, 51), bearing);
				NewUnit(commanderId, "SAM-NAGI", 80, pos(90, -45), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(119, 26), bearing);
				NewUnit(commanderId, "SAM-BOW",  80, pos(122, -4), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(165, -40), bearing);
				NewUnit(commanderId, "CAV-KATA", 40, pos(222, 30), bearing);
			}
		}
		else
		{
			if (*part == 0 || *part == 1)
			{
				NewUnit(commanderId, "SAM-NAGI", 80, pos(-130, -18), bearing);
				NewUnit(commanderId, "CAV-KATA", 40, pos(-154, -56), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(-70, 6), bearing);
				NewUnit(commanderId, "ASH-ARQ",  80, pos(-65, 77), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(-57, 44), bearing);
				NewUnit(commanderId, "ASH-BOW",  80, pos(-10, 17), bearing);
				NewUnit(commanderId, "GEN-KATA", 40, pos(-6, -27), bearing);
			}
			else if (*part == 0 || *part == 2)
			{
				NewUnit(commanderId, "SAM-NAGI", 80, pos(-5, -73), bearing);
				NewUnit(commanderId, "ASH-YARI", 80, pos(40, 45), bearing);
				NewUnit(commanderId, "ASH-ARQ",  80, pos(46, 72), bearing);
				NewUnit(commanderId, "SAM-KATA", 80, pos(50, 8), bearing);
				NewUnit(commanderId, "CAV-BOW",  40, pos(167, -34), bearing);
				NewUnit(commanderId, "CAV-YARI", 40, pos(89, -58), bearing);
				NewUnit(commanderId, "SAM-NAGI", 80, pos(104, -9), bearing);
			}
		}

		*part += 1;
	}
}


void BattleScript::Tick(double secondsSinceLastUpdate)
{
}


/***/


void BattleScript::NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing)
{
	UnitStats unitStats = SamuraiModule::GetDefaultUnitStats(unitClass);

	BattleCommander* commander = _scenario->GetCommanders()[commanderId - 1];

	Unit* unit = _simulator->AddUnit(commander, unitClass, strength, unitStats, position);
	unit->command.bearing = glm::radians(90 - bearing);
}
