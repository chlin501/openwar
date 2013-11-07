#ifndef BattleScenario_H
#define BattleScenario_H

#include <string>
#include <vector>
#include "BattleCommander.h"

class BattleScript;
class BattleSimulator;


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

	void AddCommander(const char* id, BattleCommanderType type, const char* configuration);
	const std::vector<BattleCommander*>& GetCommanders() const { return _commanders; }

	void Start(bool master);

	void SetSmoothMap(const char* name, float size);
	void SetTiledMap(int x, int y);

	void Tick(double secondsSinceLastTick);
};



#endif
