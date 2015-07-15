#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include "MonkeyScript.h"
#include "Algebra/geometry.h"
#include "BattleSimulator_v1_0_0.h"


MonkeyScript::MonkeyScript(BattleSimulator_v1_0_0* simulator) :
	_simulator{simulator}
{
}


/*static int random_int(int min, int max)
{
	return min + (int)glm::linearRand<float>(0, max - min);
}*/


void MonkeyScript::Tick(double secondsSinceLastTick)
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
	BattleObjects::Unit* result = nullptr;
	float distance = 0;
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


void MonkeyScript::IssueCommands()
{
	BattleCommander* monkeyCommander = nullptr;
	for (BattleCommander* commander : _simulator->GetCommanders())
		if (commander->GetType() == BattleCommanderType::Player)
		{
			monkeyCommander = commander;
			break;
		}

	if (monkeyCommander == nullptr)
		return;

	std::vector<BattleObjects::Unit*> monkeyUnits;
	std::vector<BattleObjects::Unit*> enemyUnits;

	for (BattleObjects::Unit* unit : _simulator->GetUnits())
		if (!unit->IsRouting())
		{
			if (unit->commander == monkeyCommander)
				monkeyUnits.push_back(unit);
			else if (unit->commander->GetTeam() != monkeyCommander->GetTeam())
				enemyUnits.push_back(unit);
		}

	if (enemyUnits.empty())
		return;

	if (monkeyUnits.empty())
		return;

	glm::vec2 playerCenter = FindClusterCenter(enemyUnits);
	glm::vec2 scriptCenter = FindClusterCenter(monkeyUnits);

	for (BattleObjects::Unit* unit : monkeyUnits)
	{
		glm::vec2 unitCenter = unit->GetCenter();

		BattleObjects::Unit* targetUnit = FindNearestUnit(enemyUnits, unitCenter);
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
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else if (dist < 0.5f * range)
			{
				glm::vec2 destination = targetCenter - 0.7f * range * glm::normalize(diff);

				BattleObjects::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				command.running = true;
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else
			{
				BattleObjects::UnitCommand command;
				command.bearing = angle(targetCenter - unitCenter);;
				_simulator->SetUnitCommand(unit, command, 0);
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
				_simulator->SetUnitCommand(unit, command, 0);
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
				_simulator->SetUnitCommand(unit, command, 0);
			}
		}
	}
}
