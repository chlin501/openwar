#include <cstdlib>
#include <cstring>
#include "Storage/Resource.h"
#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "BattleScript.h"
#include "PracticeScript.h"
#include "SmoothGroundMap.h"
#include "TiledGroundMap.h"


BattleScenario::BattleScenario()
{
	_simulator = new BattleSimulator();
	_script = new BattleScript(this);

	_dummyCommander = new BattleCommander(this, "", 1, BattleCommanderType::None);
}


BattleScenario::~BattleScenario()
{
	for (BattleCommander* commander : _commanders)
		delete commander;

	delete _script;

	GroundMap* groundMap = _simulator->GetGroundMap();
	delete _simulator;
	delete groundMap;

	delete _dummyCommander;
}


void BattleScenario::SetScript(BattleScript* value)
{
	delete _script;
	_script = value;
}


BattleScript* BattleScenario::GetScript() const
{
	return _script;
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


void BattleScenario::StartLegacyScenario()
{
	if (_script != nullptr)
		_script->Execute();
}


BattleOutcome BattleScenario::GetOutcome(int team) const
{
	int winnerTeam = _simulator->GetWinnerTeam();
	if (winnerTeam == 0)
		return BattleOutcome::None;
	return team == winnerTeam ? BattleOutcome::Won : BattleOutcome::Lost;
}


void BattleScenario::SetGroundMap(GroundMap* groundMap)
{

}


void BattleScenario::LoadLegacySmoothMap(const char* path, const char* legacyMapId, float size)
{
	if (_legacyMapId == legacyMapId)
		return;

    Resource res(path);
	if (!res.load())
	{
		res = Resource("Maps/DefaultMap.png");
		if (!res.load())
			return;
	}

	GroundMap* oldGroundMap = _simulator->GetGroundMap();

	bounds2f bounds(0, 0, size, size);
	auto smoothMap = std::unique_ptr<Image>(new Image());
	smoothMap->LoadFromResource(res);

	_simulator->SetGroundMap(new SmoothGroundMap(bounds, std::move(smoothMap)));

	delete oldGroundMap;

	_legacyMapId = legacyMapId;
}


void BattleScenario::SetTiledMap(int x, int y)
{
	bounds2f bounds(0, 0, 1024, 1024);

	GroundMap* old = _simulator->GetGroundMap();
	_simulator->SetGroundMap(new TiledGroundMap(bounds, glm::ivec2(x, y)));
	delete old;
}


const char* BattleScenario::GetLegacyMapId() const
{
	return _legacyMapId.empty() ? nullptr : _legacyMapId.c_str();
}


void BattleScenario::Tick(double secondsSinceLastTick)
{
	if (_script != nullptr)
		_script->Tick(secondsSinceLastTick);

	UpdateDeploymentZones(secondsSinceLastTick);
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
			if (deploymentOffset < 512.0f && !_simulator->HasCompletedDeployment(team))
			{
				float sign = GetTeamPosition(team) == 1 ? -1.0f : 1.0f;
				_simulator->SetDeploymentZone(team, glm::vec2{512.0f, 512.0f + sign * (deploymentRadius + deploymentOffset)}, deploymentRadius);
			}
			else
			{
				_simulator->SetDeploymentZone(team, glm::vec2{}, 0);
			}
		}

		_deploymentTimer += static_cast<float>(secondsSinceLastTick);
	}
	else
	{
		_simulator->SetDeploymentZone(1, glm::vec2{}, 0);
		_simulator->SetDeploymentZone(2, glm::vec2{}, 0);
	}
}
