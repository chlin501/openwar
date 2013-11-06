#ifndef BattleScenario_H
#define BattleScenario_H

#include <vector>
#include "BattleCommander.h"

class BattleSimulator;


class BattleScenario
{
	BattleSimulator* _simulator;
	std::vector<BattleCommander*> _commanders;

public:
	BattleScenario();
	~BattleScenario();

	BattleSimulator* GetSimulator() const { return _simulator; }

	void AddCommander(const char* id, BattleCommanderType type, const char* configuration);
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }
};



#endif
