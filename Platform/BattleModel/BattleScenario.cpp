#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "BattleScript/BattleScript.h"

#include <cstring>
#include <map>


BattleScenario::BattleScenario(BattleSimulator* battleSimulator, float executionTime) :
	_battleSimulator{battleSimulator},
	_executionTime{executionTime}
{
	_dummyCommander = new BattleCommander(this, "", 1, BattleCommanderType::None);
}


BattleScenario::~BattleScenario()
{
	for (BattleCommander* commander : _commanders)
		delete commander;

	delete _dummyCommander;
}


void BattleScenario::SetBattleScript(BattleScript* battleScript)
{
	_battleScript = battleScript;
}


void BattleScenario::Tick(float secondsSinceLastTick)
{
	if (_battleScript)
		_battleScript->Tick(secondsSinceLastTick);

	UpdateDeploymentZones(secondsSinceLastTick);

	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
		if (!unit->deployed && !IsDeploymentZone(unit->GetTeam(), unit->GetCenter()))
			unit->deployed = true;

	if (_winnerTeam == 0)
	{
		std::map<int, int> total;
		std::map<int, int> routing;

		for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
		{
			int team = unit->GetTeam();
			total[team] += 1;
			if (unit->IsRouting())
				routing[team] += 1;
		}

		for (std::pair<int, int> i : total)
		{
			int team = i.first;
			if (routing[team] == i.second)
			{
				_winnerTeam = 3 - team;
				break;
			}
		}

		if (_practice && _winnerTeam == 1)
			_winnerTeam = 0;
	}

	if (_winnerTeam != 0)
	{
		for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
			if (unit->GetTeam() != _winnerTeam)
				unit->SetIntrinsicMorale(-1.0f);
	}
}


BattleCommander* BattleScenario::AddCommander(const char* playerId, int team, BattleCommanderType type)
{
	BattleCommander* commander = new BattleCommander(this, playerId, team, type);
	_commanders.push_back(commander);
	return commander;
}


BattleCommander* BattleScenario::GetCommander(const char* playerId) const
{
	for (BattleCommander* commander : _commanders)
		if (std::strcmp(commander->GetPlayerId(), playerId) == 0)
			return commander;

	return nullptr;
}


BattleCommander* BattleScenario::GetDummyCommander() const
{
	return _dummyCommander;
}


bool BattleScenario::IsFriendlyCommander(const BattleObjects::Unit* unit, BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (unit->commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (unit->commander->GetTeam() != battleCommander->GetTeam())
		return false;

	return true;
}


bool BattleScenario::IsCommandableBy(const BattleObjects::Unit* unit, BattleCommander* battleCommander) const
{
	if (battleCommander == nullptr)
		return false;

	if (unit->commander == battleCommander)
		return true;

	if (battleCommander->GetType() == BattleCommanderType::None)
		return false;

	if (unit->commander->IsIncapacitated() && unit->commander->GetTeam() == battleCommander->GetTeam())
		return true;

	return false;
}


void BattleScenario::SetTeamPosition(int team, int position)
{
	if (team == 1)
		_teamPosition1 = position;
	if (team == 2)
		_teamPosition2 = position;
}


int BattleScenario::GetTeamPosition(int team) const
{
	if (team == 1)
		return _teamPosition1;
	if (team == 2)
		return _teamPosition2;
	return 0;
}


bool BattleScenario::IsDeployment() const
{
	return _deployment;
}


void BattleScenario::SetDeployment(bool value)
{
	_deployment = value;
	UpdateDeploymentZones(0);
}


std::pair<glm::vec2, float> BattleScenario::GetDeploymentZone(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1];
}


bool BattleScenario::IsDeploymentZone(int team, glm::vec2 position) const
{
	std::pair<glm::vec2, float> deploymentZone = GetDeploymentZone(team);
	return glm::distance(position, deploymentZone.first) < deploymentZone.second;
}


glm::vec2 BattleScenario::ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const
{
	std::pair<glm::vec2, float> deploymentZone = GetDeploymentZone(team);
	float radius = deploymentZone.second - inset;
	if (radius > 0)
	{
		glm::vec2 center = deploymentZone.first;
		glm::vec2 offset = position - center;
		if (glm::length(offset) > radius)
			position = center + radius * glm::normalize(offset);
	}
	return position;
}


void BattleScenario::UpdateDeploymentZones(float secondsSinceLastTick)
{
	if (_deployment)
	{
		const float deploymentRadius = 1024.0f;
		const float deploymenyStart = 128.0f;
		const float deploymentPause = 15.0f; // seconds
		const float deploymentDuration = 45.0f; // seconds (total duration including pause)

		float deploymentOffset = deploymenyStart;
		if (_executionTime > deploymentPause)
			deploymentOffset += (_executionTime - deploymentPause) * (512.0f - deploymenyStart) / (deploymentDuration - deploymentPause);

		for (int team = 1; team <= 2; ++team)
		{
			if (deploymentOffset < 512.0f && !HasCompletedDeployment(team))
			{
				int pos = GetTeamPosition(team);
				float sign = pos == 0 ? (team == 1 ? -1.0f : 1.0f) : (pos == 1 ? -1.0f : 1.0f);
				SetDeploymentZone(team, glm::vec2{512.0f, 512.0f + sign * (deploymentRadius + deploymentOffset)}, deploymentRadius);
			}
			else
			{
				SetDeploymentZone(team, glm::vec2{}, 0);
			}
		}
	}
	else
	{
		SetDeploymentZone(1, glm::vec2{}, 0);
		SetDeploymentZone(2, glm::vec2{}, 0);
	}

	_executionTime += secondsSinceLastTick;
}


void BattleScenario::SetDeploymentZone(int team, glm::vec2 center, float radius)
{
	_deploymentZones[team == 1 ? 0 : 1] = std::make_pair(center, radius);
}


bool BattleScenario::HasCompletedDeployment(int team) const
{
	int count = 0;
	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
	{
		if (team == unit->GetTeam())
		{
			if (!unit->deployed)
				return false;
			++count;
		}
	}

	return count != 0;
}
