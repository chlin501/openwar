#include <cstdlib>
#include "../Library/resource.h"
#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "BattleScript.h"
#include "SamuraiModule.h"
#include "SmoothGroundMap.h"
#include "TiledGroundMap.h"


BattleScenario::BattleScenario(const char* name) :
_name(name),
_simulator(nullptr),
_script(nullptr),
_smoothMap(nullptr)
{
	_simulator = new BattleSimulator();
	_script = new BattleScript(this);
}


BattleScenario::~BattleScenario()
{
	for (BattleCommander* commander : _commanders)
		delete commander;

	GroundMap* groundMap = _simulator->GetGroundMap();
	delete _simulator;
	delete groundMap;

	delete _smoothMap;
}


BattleCommander* BattleScenario::AddCommander(const char* id, int team, BattleCommanderType type, const char* configuration)
{
	BattleCommander* commander = new BattleCommander(id, team, type, configuration);
	_commanders.push_back(commander);
	return commander;
}


BattleCommander* BattleScenario::GetCommander(const char* id) const
{
	for (BattleCommander* commander : _commanders)
		if (std::strcmp(commander->GetId(), id) == 0)
			return commander;
	return nullptr;
}


void BattleScenario::StartScript(bool master)
{
	if (!_name.empty())
	{
		_script->AddPackagePath(resource("Scripts/?.lua").path());
		_script->AddPackagePath(resource("Maps/?.lua").path());
		_script->SetGlobalBoolean("openwar_is_master", master);
		_script->SetCommanders("openwar_commanders");

		resource source(_name.c_str());
		if (source.load())
		{
			_script->Execute((const char*)source.data(), source.size());
		}
	}
}


BattleOutcome BattleScenario::GetOutcome(int team) const
{
	int winnerTeam = _simulator->GetWinnerTeam();
	if (winnerTeam == 0)
		return BattleOutcome::None;
	return team == winnerTeam ? BattleOutcome::Won : BattleOutcome::Lost;
}


void BattleScenario::SetSmoothMap(const char* name, float size)
{
	std::string path = std::string("Maps/") + name;

	image* oldSmoothMap = _smoothMap;
	GroundMap* oldGroundMap = _simulator->GetGroundMap();

	bounds2f bounds(0, 0, size, size);
	_smoothMap = new image(resource(path.c_str()));
	_simulator->SetGroundMap(new SmoothGroundMap(_simulator->GetHeightMap(), name, bounds, _smoothMap));

	delete oldGroundMap;
	delete oldSmoothMap;
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
	_script->Tick(secondsSinceLastTick);
}
