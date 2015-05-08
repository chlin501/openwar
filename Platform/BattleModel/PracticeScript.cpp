#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include "PracticeScript.h"
#include "Algebra/geometry.h"


PracticeScript::PracticeScript(BattleSimulator* simulator) : BattleScript{simulator}
{
}


PracticeScript::~PracticeScript()
{
}


void PracticeScript::Execute()
{
	_simulator->SetPractice(true);

	glm::vec2 center(512, 512);

	_simulator->NewUnit(1, "SAM-BOW", 80, center + glm::vec2(-50, 0), 0);
	_simulator->NewUnit(1, "SAM-ARQ", 80, center + glm::vec2(  0, 0), 0);
	_simulator->NewUnit(1, "SAM-BOW", 80, center + glm::vec2( 50, 0), 0);

	_simulator->NewUnit(1, "SAM-YARI", 80, center + glm::vec2(-25, -30), 0);
	_simulator->NewUnit(1, "SAM-YARI", 80, center + glm::vec2( 25, -30), 0);

	_simulator->NewUnit(1, "SAM-KATA", 80, center + glm::vec2(-50, -60), 0);
	_simulator->NewUnit(1, "GEN-KATA", 40, center + glm::vec2(  0, -60), 0);
	_simulator->NewUnit(1, "SAM-KATA", 80, center + glm::vec2( 50, -60), 0);

	_simulator->NewUnit(1, "CAV-YARI", 40, center + glm::vec2(-70, -100), 0);
	_simulator->NewUnit(1, "SAM-NAGI", 80, center + glm::vec2(  0, -90), 0);
	_simulator->NewUnit(1, "CAV-BOW",  40, center + glm::vec2( 70, -100), 0);

	SpawnWave();
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


static Unit* FindNearestUnit(const std::vector<Unit*>& units, glm::vec2 position)
{
    Unit* result{};
    float distance{};
	for (Unit* unit : units)
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


static Unit* FindCenterUnit(const std::vector<Unit*>& units)
{
	if (units.empty())
		return nullptr;

	std::vector<std::pair<Unit*, float>> items;
	for (Unit* unit : units)
	{
		float weight = 0;
		for (Unit* u : units)
			if (u != unit)
				weight += 1.0f / (1.0f + glm::distance(u->state.center, unit->state.center));
		items.push_back(std::pair<Unit*, float>(unit, weight));
	}
	std::sort(items.begin(), items.end(), [](std::pair<Unit*, float> a, std::pair<Unit*, float> b) {
		return a.second < b.second;
	});

	return items.back().first;
}


static glm::vec2 FindClusterCenter(const std::vector<Unit*>& units)
{
	if (units.empty())
		return glm::vec2();

	Unit* centerUnit = FindCenterUnit(units);
	glm::vec2 result;
	float weight = 0;

	for (Unit* unit : units)
	{
		float w = 1.0f / (50.0f + glm::distance(unit->state.center, centerUnit->state.center));
		result += w * unit->state.center;
		weight += w;
	}

	return result / weight;
}


void PracticeScript::IssueCommands()
{
	std::vector<Unit*> playerUnits;
	std::vector<Unit*> scriptUnits;

	for (Unit* unit : _simulator->GetUnits())
		if (!unit->state.IsRouting())
		{
			std::vector<Unit*>& unitList = unit->commander->GetTeam() == 1 ? playerUnits : scriptUnits;
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

 	for (Unit* unit : scriptUnits)
	{
		glm::vec2 unitCenter = unit->state.center;

		Unit* targetUnit = FindNearestUnit(playerUnits, unitCenter);
		if (targetUnit == nullptr)
			continue;

		glm::vec2 targetCenter = targetUnit->state.center;

		if (unit->stats.missileType != MissileType::None)
		{
			float range = unit->stats.maximumRange;
			glm::vec2 diff = targetCenter - unitCenter;
			float dist = glm::length(diff);
			if (dist > 0.9f * range)
			{
				glm::vec2 destination = targetCenter - 0.9f * range * glm::normalize(diff);

				UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else if (dist < 0.5f * range)
			{
				glm::vec2 destination = targetCenter - 0.7f * range * glm::normalize(diff);

				UnitCommand command;
				command.path.push_back(unitCenter);
				command.path.push_back(destination);
				command.bearing = angle(destination - unitCenter);
				command.running = true;
				_simulator->SetUnitCommand(unit, command, 0);
			}
			else
			{
				UnitCommand command;
				command.bearing = angle(targetCenter - unitCenter);;
				_simulator->SetUnitCommand(unit, command, 0);
			}
		}
		else
		{
			if (glm::distance(targetUnit->state.center, unitCenter) < 80)
			{

				UnitCommand command;
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

				UnitCommand command;
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
	std::vector<Unit*> playerUnits;

	for (Unit* unit : _simulator->GetUnits())
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
