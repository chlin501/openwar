#include <cstdlib>
#include "../Library/resource.h"
#include "../Library/Algebra/image.h"
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
_script(nullptr)
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
}


BattleCommander* BattleScenario::AddCommander(int team, BattleCommanderType type, const char* configuration)
{
	BattleCommander* commander = new BattleCommander(team, type, configuration);
	_commanders.push_back(commander);
	return commander;
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


void BattleScenario::SetSmoothMap(const char* name, float size)
{
	std::string path = std::string("Maps/") + name;
	image* map = new image(resource(path.c_str()));
	bounds2f bounds(0, 0, size, size);

	GroundMap* old = _simulator->GetGroundMap();
	_simulator->SetGroundMap(new SmoothGroundMap(_simulator->GetHeightMap(), name, bounds, map));
	delete old;
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
