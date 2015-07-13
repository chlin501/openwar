#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include "MonkeyScript.h"
#include "Algebra/geometry.h"
#include "BattleSimulator.h"


MonkeyScript::MonkeyScript(BattleSimulator* simulator) :
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


static BattleObjects_v1::Unit* FindNearestUnit(const std::vector<BattleObjects_v1::Unit*>& units, glm::vec2 position)
{
	BattleObjects_v1::Unit* result = nullptr;
	float distance = 0;
	for (BattleObjects_v1::Unit* unit : units)
	{
		float d = glm::distance(position, unit->state.center);
		if (result == nullptr || d < distance)
		{
			result = unit;
			distance = d;
		}
	}
	return result;
}


static BattleObjects_v1::Unit* FindCenterUnit(const std::vector<BattleObjects_v1::Unit*>& units)
{
	if (units.empty())
		return nullptr;

	std::vector<std::pair<BattleObjects_v1::Unit*, float>> items;
	for (BattleObjects_v1::Unit* unit : units)
	{
		float weight = 0;
		for (BattleObjects_v1::Unit* u : units)
			if (u != unit)
				weight += 1.0f / (1.0f + glm::distance(u->state.center, unit->state.center));
		items.push_back(std::pair<BattleObjects_v1::Unit*, float>(unit, weight));
	}
	std::sort(items.begin(), items.end(), [](std::pair<BattleObjects_v1::Unit*, float> a, std::pair<BattleObjects_v1::Unit*, float> b) {
		return a.second < b.second;
	});

	return items.back().first;
}


static glm::vec2 FindClusterCenter(const std::vector<BattleObjects_v1::Unit*>& units)
{
	if (units.empty())
		return glm::vec2();

	BattleObjects_v1::Unit* centerUnit = FindCenterUnit(units);
	glm::vec2 result;
	float weight = 0;

	for (BattleObjects_v1::Unit* unit : units)
	{
		float w = 1.0f / (50.0f + glm::distance(unit->state.center, centerUnit->state.center));
		result += w * unit->state.center;
		weight += w;
	}

	return result / weight;
}


void MonkeyScript::IssueCommands()
{
	BattleObjects::Commander* monkeyCommander = nullptr;
	for (BattleObjects::Commander* commander : _simulator->GetCommanders())
		if (commander->GetType() == BattleObjects::CommanderType::Player)
		{
			monkeyCommander = commander;
			break;
		}

	if (monkeyCommander == nullptr)
		return;

	std::vector<BattleObjects_v1::Unit*> monkeyUnits;
	std::vector<BattleObjects_v1::Unit*> enemyUnits;

	for (BattleObjects_v1::Unit* unit : _simulator->GetUnits())
		if (!unit->state.IsRouting())
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

	for (BattleObjects_v1::Unit* unit : monkeyUnits)
	{
		glm::vec2 unitCenter = unit->state.center;

		BattleObjects_v1::Unit* targetUnit = FindNearestUnit(enemyUnits, unitCenter);
		if (targetUnit == nullptr)
			continue;

		glm::vec2 targetCenter = targetUnit->state.center;

		if (unit->stats.missileType != BattleObjects_v1::MissileType::None)
		{
			float range = unit->stats.maximumRange;
			glm::vec2 diff = targetCenter - unitCenter;
			float dist = glm::length(diff);
			if (dist > 0.9f * range)
			{
				glm::vec2 destination = targetCenter - 0.9f * range * glm::normalize(diff);

				BattleObjects_v1::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else if (dist < 0.5f * range)
			{
				glm::vec2 destination = targetCenter - 0.7f * range * glm::normalize(diff);

				BattleObjects_v1::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				command.running = true;
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else
			{
				BattleObjects_v1::UnitCommand command;
				command.bearing = angle(targetCenter - unitCenter);;
				_simulator->SetUnitCommand(unit, command, 0);
			}
		}
		else
		{
			if (glm::distance(targetUnit->state.center, unitCenter) < 80)
			{
				BattleObjects_v1::UnitCommand command;
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

				BattleObjects_v1::UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				_simulator->SetUnitCommand(unit, command, 0);
			}
		}
	}
}
