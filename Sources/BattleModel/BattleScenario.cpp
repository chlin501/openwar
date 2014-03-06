#include <cstdlib>
#include "../Library/resource.h"
#include "GroundMap.h"
#include "BattleScenario.h"
#include "BattleSimulator.h"
#include "BattleScript.h"
#include "SamuraiModule.h"
#include "SmoothGroundMap.h"
#include "TiledGroundMap.h"


BattleScenario::BattleScenario() :
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

	delete _script;

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
	_script->SetIsMaster(master);
	_script->Execute();
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
	if (_smoothMapName == name)
		return;

	std::string path = std::string("Maps/") + name;
    resource res(path.c_str());
    if (!res.load())
    {
        res = resource("Maps/DefaultMap.tiff");
        if (!res.load())
            return;
    }
    
	image* oldSmoothMap = _smoothMap;
	GroundMap* oldGroundMap = _simulator->GetGroundMap();

	bounds2f bounds(0, 0, size, size);
	_smoothMap = new image(res);
	_simulator->SetGroundMap(new SmoothGroundMap(_simulator->GetHeightMap(), name, bounds, _smoothMap));

	delete oldGroundMap;
	delete oldSmoothMap;

	_smoothMapName = name;
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
