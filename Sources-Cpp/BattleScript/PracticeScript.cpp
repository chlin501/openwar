#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include "PracticeScript.h"
#include "Algebra/geometry.h"
#include "BattleModel/BattleScenario.h"
#include "BattleModel/BattleSimulator.h"


PracticeScript::PracticeScript(BattleScenario* battleScenario) :
	_battleScenario{battleScenario},
	_battleSimulator{battleScenario->GetBattleSimulator()}
{
}


void PracticeScript::Tick(double secondsSinceLastTick)
{
	_commandTimer -= secondsSinceLastTick;
	if (_commandTimer <= 0)
	{
		IssueCommands();
		_commandTimer = 1;
	}
}


static BattleObjects::Unit* FindNearestUnit(const std::vector<BattleObjects::Unit*>& units, glm::vec2 position)
{
	BattleObjects::Unit* result{};
    float distance{};
	for (BattleObjects::Unit* unit : units)
	{
		float d = glm::distance(position, unit->GetCenter());
		if (result == nullptr || d < distance)
		{
			result = unit;
			distance = d;
		}
	}
	return result;
}


static BattleObjects::Unit* FindCenterUnit(const std::vector<BattleObjects::Unit*>& units)
{
	if (units.empty())
		return nullptr;

	std::vector<std::pair<BattleObjects::Unit*, float>> items;
	for (BattleObjects::Unit* unit : units)
	{
		float weight = 0;
		for (BattleObjects::Unit* u : units)
			if (u != unit)
				weight += 1.0f / (1.0f + glm::distance(u->GetCenter(), unit->GetCenter()));
		items.push_back(std::pair<BattleObjects::Unit*, float>(unit, weight));
	}
	std::sort(items.begin(), items.end(), [](std::pair<BattleObjects::Unit*, float> a, std::pair<BattleObjects::Unit*, float> b) {
		return a.second < b.second;
	});

	return items.back().first;
}


static glm::vec2 FindClusterCenter(const std::vector<BattleObjects::Unit*>& units)
{
	if (units.empty())
		return glm::vec2();

	BattleObjects::Unit* centerUnit = FindCenterUnit(units);
	glm::vec2 result;
	float weight = 0;

	for (BattleObjects::Unit* unit : units)
	{
		float w = 1.0f / (50.0f + glm::distance(unit->GetCenter(), centerUnit->GetCenter()));
		result += w * unit->GetCenter();
		weight += w;
	}

	return result / weight;
}


void PracticeScript::IssueCommands()
{
	std::vector<BattleObjects::Unit*> playerUnits;
	std::vector<BattleObjects::Unit*> scriptUnits;

	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
		if (!unit->IsRouting())
		{
			std::vector<BattleObjects::Unit*>& unitList = unit->GetTeam() == 1 ? playerUnits : scriptUnits;
			unitList.push_back(unit);
		}

	if (playerUnits.empty())
		return;

	if (scriptUnits.empty())
	{
		SpawnWave();
		return;
	}

	glm::vec2 playerCenter = FindClusterCenter(playerUnits);
	glm::vec2 scriptCenter = FindClusterCenter(scriptUnits);

 	for (BattleObjects::Unit* unit : scriptUnits)
	{
		glm::vec2 unitCenter = unit->GetCenter();

		BattleObjects::Unit* targetUnit = FindNearestUnit(playerUnits, unitCenter);
		if (targetUnit == nullptr)
			continue;

		glm::vec2 targetCenter = targetUnit->GetCenter();

		if (unit->GetMissileWeaponRange().maximumRange > 0)
		{
			float range = unit->GetMissileWeaponRange().maximumRange;
			glm::vec2 diff = targetCenter - unitCenter;
			float dist = glm::length(diff);
			if (dist > 0.9f * range)
			{
				glm::vec2 destination = targetCenter - 0.9f * range * glm::normalize(diff);

				BattleObjects::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				_battleSimulator->SetUnitCommand(unit, command, 0);
			}
			else if (dist < 0.5f * range)
			{
				glm::vec2 destination = targetCenter - 0.7f * range * glm::normalize(diff);

				BattleObjects::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				command.running = true;
				_battleSimulator->SetUnitCommand(unit, command, 0);
			}
			else
			{
				BattleObjects::UnitCommand command;
				command.bearing = angle(targetCenter - unitCenter);;
				_battleSimulator->SetUnitCommand(unit, command, 0);
			}
		}
		else
		{
			if (glm::distance(targetUnit->GetCenter(), unitCenter) < 80)
			{

				BattleObjects::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(targetCenter);
				command.bearing = angle(targetCenter - unitCenter);
				_battleSimulator->SetUnitCommand(unit, command, 0);
			}
			else
			{
				glm::vec2 diff = unitCenter - scriptCenter;
				float dist = glm::length(diff);
				if (dist > 100)
					diff *= 100 / dist;

				glm::vec2 destination = playerCenter + diff;

				BattleObjects::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				_battleSimulator->SetUnitCommand(unit, command, 0);
			}
		}
	}
}


void PracticeScript::SpawnWave()
{
	std::vector<BattleObjects::Unit*> playerUnits;

	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
		if (unit->GetTeam() == 1 && !unit->IsRouting())
			playerUnits.push_back(unit);

	glm::vec2 playerCenter = FindClusterCenter(playerUnits);

	glm::vec2 direction = glm::vec2(512, 512) - playerCenter;

	if (glm::length(direction) > 1)
		direction = glm::normalize(direction);
	else
		direction = glm::vec2(0, 1);

	glm::vec2 p =  playerCenter + 200.0f * direction;

	float a = glm::atan(direction.y, direction.x) +  glm::half_pi<float>();
	float bearing = glm::radians(90.0f) - a;

	switch (_waveNumber)
	{
		case 0:
			AddEnemyUnit("ASH-YARI", 80, p + rotate(glm::vec2(-90, 0), a), bearing);
			AddEnemyUnit("ASH-YARI", 80, p + rotate(glm::vec2(-30, 0), a), bearing);
			AddEnemyUnit("ASH-YARI", 80, p + rotate(glm::vec2(30, 0), a), bearing);
			AddEnemyUnit("ASH-YARI", 80, p + rotate(glm::vec2(90, 0), a), bearing);
			break;

		case 1:
			AddEnemyUnit("ASH-BOW", 80, p + rotate(glm::vec2(-40, 0), a), bearing);
			AddEnemyUnit("ASH-BOW", 80, p + rotate(glm::vec2(40, 0), a), bearing);
			break;

		case 2:
			AddEnemyUnit("SAM-KATA", 80, p + rotate(glm::vec2(-60, 0), a), bearing);
			AddEnemyUnit("SAM-NAGI", 80, p + rotate(glm::vec2(0, 0), a), bearing);
			AddEnemyUnit("SAM-KATA", 80, p + rotate(glm::vec2(60, 0), a), bearing);
			break;

		case 3:
			AddEnemyUnit("CAV-BOW", 40, p + rotate(glm::vec2(-60, 0), a), bearing);
			AddEnemyUnit("CAV-BOW", 40, p + rotate(glm::vec2(60, 0), a), bearing);
			break;

		case 4:
			AddEnemyUnit("CAV-YARI", 40, p + rotate(glm::vec2(-90, 0), a), bearing);
			AddEnemyUnit("SAM-KATA", 80, p + rotate(glm::vec2(-30, 0), a), bearing);
			AddEnemyUnit("SAM-KATA", 80, p + rotate(glm::vec2(30, 0), a), bearing);
			AddEnemyUnit("CAV-YARI", 40, p + rotate(glm::vec2(90, 0), a), bearing);
			break;

		case 5:
			AddEnemyUnit("ASH-ARQ", 80, p + rotate(glm::vec2(-60, 0), a), bearing);
			AddEnemyUnit("ASH-ARQ", 80, p + rotate(glm::vec2(0, 0), a), bearing);
			AddEnemyUnit("ASH-ARQ", 80, p + rotate(glm::vec2(60, 0), a), bearing);
			break;
	}

	if (++_waveNumber == 6)
		_waveNumber = 0;
}



void PracticeScript::SpawnPlayerUnits()
{
	glm::vec2 center(512, 512);
	float bearing = glm::radians(90.0f);

	AddPlayerUnit("SAM-BOW", 80, center + glm::vec2(-50, 0), bearing);
	AddPlayerUnit("SAM-ARQ", 80, center + glm::vec2(0, 0), bearing);
	AddPlayerUnit("SAM-BOW", 80, center + glm::vec2(50, 0), bearing);

	AddPlayerUnit("SAM-YARI", 80, center + glm::vec2(-25, -30), bearing);
	AddPlayerUnit("SAM-YARI", 80, center + glm::vec2(25, -30), bearing);

	AddPlayerUnit("SAM-KATA", 80, center + glm::vec2(-50, -60), bearing);
	AddPlayerUnit("GEN-KATA", 40, center + glm::vec2(0, -60), bearing);
	AddPlayerUnit("SAM-KATA", 80, center + glm::vec2(50, -60), bearing);

	AddPlayerUnit("CAV-YARI", 40, center + glm::vec2(-70, -100), bearing);
	AddPlayerUnit("SAM-NAGI", 80, center + glm::vec2(0, -90), bearing);
	AddPlayerUnit("CAV-BOW", 40, center + glm::vec2(70, -100), bearing);
}


void PracticeScript::AddPlayerUnit(const char* unitClass, int numberOfFighters, glm::vec2 position, float bearing)
{
	BattleCommander* commander = _battleScenario->GetCommanders()[0];
	BattleObjects::Unit* unit = _battleSimulator->AddUnit(commander, unitClass, numberOfFighters, position, bearing);
	unit->SetOwnedBySimulator(true);
}


void PracticeScript::AddEnemyUnit(const char* unitClass, int numberOfFighters, glm::vec2 position, float bearing)
{
	BattleCommander* commander = _battleScenario->GetCommanders()[1];
	BattleObjects::Unit* unit = _battleSimulator->AddUnit(commander, unitClass, numberOfFighters, position, bearing);
	unit->SetOwnedBySimulator(true);
	unit->canRally = false;
}


