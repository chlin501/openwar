#include "BattleScenario.h"
#include "BattleSimulator.h"
#import "BattleObjects_v1.h"


BattleScenario::BattleScenario(BattleSimulator* battleSimulator) :
	_battleSimulator(battleSimulator)
{

}


BattleScenario::~BattleScenario()
{
}


void BattleScenario::Tick(float secondsSinceLastTick)
{
	UpdateDeploymentZones(secondsSinceLastTick);

	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
		if (!unit->deployed && !IsDeploymentZone(unit->commander->GetTeam(), unit->GetCenter()))
			unit->deployed = true;
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




void BattleScenario::EnableDeploymentZones(float deploymentTimer)
{
	_deploymentTimer = deploymentTimer;
	_deploymentEnabled = true;
	UpdateDeploymentZones(0);
}


void BattleScenario::UpdateDeploymentZones(double secondsSinceLastTick)
{
	if (_deploymentEnabled)
	{
		const float deploymentRadius = 1024.0f;
		const float deploymenyStart = 128.0f;
		const float deploymentPause = 15.0f; // seconds
		const float deploymentDuration = 45.0f; // seconds (total duration including pause)

		float deploymentOffset = deploymenyStart;
		if (_deploymentTimer > deploymentPause)
			deploymentOffset += (_deploymentTimer - deploymentPause) * (512.0f - deploymenyStart) / (deploymentDuration - deploymentPause);

		for (int team = 1; team <= 2; ++team)
		{
			if (deploymentOffset < 512.0f && !HasCompletedDeployment(team))
			{
				float sign = _battleSimulator->GetTeamPosition(team) == 1 ? -1.0f : 1.0f;
				SetDeploymentZone(team, glm::vec2{512.0f, 512.0f + sign * (deploymentRadius + deploymentOffset)}, deploymentRadius);
			}
			else
			{
				SetDeploymentZone(team, glm::vec2{}, 0);
			}
		}

		_deploymentTimer += static_cast<float>(secondsSinceLastTick);
	}
	else
	{
		SetDeploymentZone(1, glm::vec2{}, 0);
		SetDeploymentZone(2, glm::vec2{}, 0);
	}
}


void BattleScenario::SetDeploymentZone(int team, glm::vec2 center, float radius)
{
	_deploymentZones[team == 1 ? 0 : 1] = std::make_pair(center, radius);
};


std::pair<glm::vec2, float> BattleScenario::GetDeploymentZone(int team) const
{
	return _deploymentZones[team == 1 ? 0 : 1];
}


bool BattleScenario::HasCompletedDeployment(int team) const
{
	int count = 0;
	for (BattleObjects::Unit* unit : _battleSimulator->GetUnits())
	{
		if (team == unit->commander->GetTeam())
		{
			if (!unit->deployed)
				return false;
			++count;
		}
	}

	return count != 0;
}
