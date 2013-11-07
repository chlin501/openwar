#ifndef BattleScenario_H
#define BattleScenario_H

#include <vector>
#include "BattleCommander.h"

class BattleScript;
class BattleSimulator;


class BattleScenario
{
	BattleSimulator* _simulator;
	BattleScript* _script;
	std::vector<BattleCommander*> _commanders;

public:
	BattleScenario();
	~BattleScenario();

	BattleSimulator* GetSimulator() const { return _simulator; }
	BattleScript* GetScript() const { return _script; }

	void Tick(double secondsSinceLastTick);

	void AddCommander(const char* id, BattleCommanderType type, const char* configuration);
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }

	void AddUnits();
};



#endif
