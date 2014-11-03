#include <cstdlib>
#include <cstring>
#include "resource.h"
#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "BattleScript.h"
#include "PracticeScript.h"
#include "SmoothGroundMap.h"
#include "TiledGroundMap.h"


BattleScenario::BattleScenario() :
_simulator(nullptr),
_script(nullptr),
_smoothMap(nullptr),
_teamPosition1(0),
_teamPosition2(0)

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

	delete _smoothMap;

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


void BattleScenario::StartScript()
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


void BattleScenario::SetSmoothMap(const char* path, const char* hash, float size)
{
	if (_smoothMapHash == hash)
		return;

    resource res(path);
	if (!res.load())
	{
		res = resource("Maps/DefaultMap.png");
		if (!res.load())
			return;
	}

	Image* oldSmoothMap = _smoothMap;
	GroundMap* oldGroundMap = _simulator->GetGroundMap();

	bounds2f bounds(0, 0, size, size);
	_smoothMap = new Image();
	_smoothMap->LoadFromResource(res);

	_simulator->SetGroundMap(new SmoothGroundMap(_simulator->GetHeightMap(), hash, bounds, _smoothMap));

	delete oldGroundMap;
	delete oldSmoothMap;

	_smoothMapHash = hash;
}


void BattleScenario::SetTiledMap(int x, int y)
{
	bounds2f bounds(0, 0, 1024, 1024);

	GroundMap* old = _simulator->GetGroundMap();
	_simulator->SetGroundMap(new TiledGroundMap(_simulator->GetHeightMap(), bounds, glm::ivec2(x, y)));
	delete old;
}


void BattleScenario::Tick(double secondsSinceLastTick)
{
	if (_script != nullptr)
		_script->Tick(secondsSinceLastTick);
}
