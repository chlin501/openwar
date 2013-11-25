#ifndef BattleScenario_H
#define BattleScenario_H

#include <string>
#include <vector>
#include "BattleCommander.h"

class BattleScript;
class BattleSimulator;


enum class BattleOutcome
{
	None,
	Won,
	Lost,
	Draw
};


class BattleScenario
{
	std::string _name;
	BattleSimulator* _simulator;
	BattleScript* _script;
	std::vector<BattleCommander*> _commanders;

public:
	BattleScenario(const char* name);
	~BattleScenario();

	BattleSimulator* GetSimulator() const { return _simulator; }
	BattleScript* GetScript() const { return _script; }

	BattleCommander* AddCommander(const char* id, int team, BattleCommanderType type, const char* configuration);
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }

	void StartScript(bool master);

	BattleOutcome GetOutcome(int team) const;

	void SetSmoothMap(const char* name, float size);
	void SetTiledMap(int x, int y);

	void Tick(double secondsSinceLastTick);
};



#endif
