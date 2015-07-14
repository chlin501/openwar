#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include "PracticeScript.h"
#include "Algebra/geometry.h"
#include "BattleSimulator_v1_0_0.h"


PracticeScript::PracticeScript(BattleSimulator_v1_0_0* simulator) :
	_simulator{simulator}
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


static BattleObjects_v1::Unit* FindNearestUnit(const std::vector<BattleObjects_v1::Unit*>& units, glm::vec2 position)
{
	BattleObjects_v1:: Unit* result{};
    float distance{};
	for (BattleObjects_v1::Unit* unit : units)
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
				weight += 1.0f / (1.0f + glm::distance(u->GetCenter(), unit->GetCenter()));
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
		float w = 1.0f / (50.0f + glm::distance(unit->GetCenter(), centerUnit->GetCenter()));
		result += w * unit->GetCenter();
		weight += w;
	}

	return result / weight;
}


void PracticeScript::IssueCommands()
{
	std::vector<BattleObjects_v1::Unit*> playerUnits;
	std::vector<BattleObjects_v1::Unit*> scriptUnits;

	for (BattleObjects_v1::Unit* unit : _simulator->GetUnits())
		if (!unit->state.IsRouting())
		{
			std::vector<BattleObjects_v1::Unit*>& unitList = unit->commander->GetTeam() == 1 ? playerUnits : scriptUnits;
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

 	for (BattleObjects_v1::Unit* unit : scriptUnits)
	{
		glm::vec2 unitCenter = unit->GetCenter();

		BattleObjects_v1::Unit* targetUnit = FindNearestUnit(playerUnits, unitCenter);
		if (targetUnit == nullptr)
			continue;

		glm::vec2 targetCenter = targetUnit->GetCenter();

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
			if (glm::distance(targetUnit->GetCenter(), unitCenter) < 80)
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


void PracticeScript::SpawnWave()
{
	std::vector<BattleObjects_v1::Unit*> playerUnits;

	for (BattleObjects_v1::Unit* unit : _simulator->GetUnits())
		if (unit->commander->GetTeam() == 1 && !unit->state.IsRouting())
			playerUnits.push_back(unit);

	glm::vec2 playerCenter = FindClusterCenter(playerUnits);

	glm::vec2 direction = glm::vec2(512, 512) - playerCenter;

	if (glm::length(direction) > 1)
		direction = glm::normalize(direction);
	else
		direction = glm::vec2(0, 1);

	glm::vec2 p =  playerCenter + 200.0f * direction;

	float a = glm::atan(direction.y, direction.x) +  glm::half_pi<float>();
	float bearing = glm::degrees(a);

	switch (_waveNumber)
	{
		case 0:
			_simulator->NewUnit(2, "ASH-YARI", 80, p + rotate(glm::vec2(-90, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-YARI", 80, p + rotate(glm::vec2(-30, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-YARI", 80, p + rotate(glm::vec2( 30, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-YARI", 80, p + rotate(glm::vec2( 90, 0), a), bearing);
			break;

		case 1:
			_simulator->NewUnit(2, "ASH-BOW", 80, p + rotate(glm::vec2(-40, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-BOW", 80, p + rotate(glm::vec2( 40, 0), a), bearing);
			break;

		case 2:
			_simulator->NewUnit(2, "SAM-KATA", 80, p + rotate(glm::vec2(-60, 0), a), bearing);
			_simulator->NewUnit(2, "SAM-NAGI", 80, p + rotate(glm::vec2( 0, 0), a), bearing);
			_simulator->NewUnit(2, "SAM-KATA", 80, p + rotate(glm::vec2(60, 0), a), bearing);
			break;

		case 3:
			_simulator->NewUnit(2, "CAV-BOW", 40, p + rotate(glm::vec2(-60, 0), a), bearing);
			_simulator->NewUnit(2, "CAV-BOW", 40, p + rotate(glm::vec2(60, 0), a), bearing);
			break;

		case 4:
			_simulator->NewUnit(2, "CAV-YARI", 40, p + rotate(glm::vec2(-90, 0), a), bearing);
			_simulator->NewUnit(2, "SAM-KATA", 80, p + rotate(glm::vec2(-30, 0), a), bearing);
			_simulator->NewUnit(2, "SAM-KATA", 80, p + rotate(glm::vec2( 30, 0), a), bearing);
			_simulator->NewUnit(2, "CAV-YARI", 40, p + rotate(glm::vec2( 90, 0), a), bearing);
			break;

		case 5:
			_simulator->NewUnit(2, "ASH-ARQ", 80, p + rotate(glm::vec2(-60, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-ARQ", 80, p + rotate(glm::vec2(  0, 0), a), bearing);
			_simulator->NewUnit(2, "ASH-ARQ", 80, p + rotate(glm::vec2( 60, 0), a), bearing);
			break;
	}

	if (++_waveNumber == 6)
		_waveNumber = 0;
}
